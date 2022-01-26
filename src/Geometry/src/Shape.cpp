#include "Geometry.h"
#include "Log.h"

using namespace Core;
using namespace Geometry;

Geometry::Shape::Shape()
{}

Shape::~Shape()
{
    Clear();
}

Shape::Shape(const Shape & other)
{
    Copy(other);
}

Shape & Shape::Copy(const Shape & other)
{
    Clear();

    auto normalsMap = m_normals.InsertCopy(other.m_normals);
    auto nodeMap = m_nodes.InsertCopy(other.m_nodes);
    auto edgeMap = m_edges.InsertCopy(other.m_edges);
    auto faceMap = m_faces.InsertCopy(other.m_faces);
    
    for(auto edges:edgeMap)
    {
        edges.second->MapPtrsAfterStoreCopy(nodeMap,edgeMap,faceMap);
    }
    for(auto faces:faceMap)
    {
        faces.second->MapPtrsAfterStoreCopy(this,edgeMap,normalsMap);
        m_surface.emplace_back(faces.second);
    }
    
    return *this;
}

void Shape::Clear()
{
    m_normals.ClearWithoutCallingDestructors();
    m_nodes.ClearWithoutCallingDestructors();
    m_edges.ClearWithoutCallingDestructors();
    m_faces.ClearWithoutCallingDestructors();
    m_surface.clear();
}

double Shape::CalculateVolume() const
{
    double res = 0;
    for(auto face:m_surface)
    {
        double surface = face->CalcSurface();
        auto & normal = face->CalcNormal();
        res += (surface/9)*(face->GetEdge(0)->Start()->InnerProduct(normal)
                           +face->GetEdge(1)->Start()->InnerProduct(normal)
                           +face->GetEdge(2)->Start()->InnerProduct(normal));
    }
    return res;
}

Shape Shape::Construct::Cube(const double side)
{
    return Box({side,side,side});
}

Shape Shape::Construct::Box(const Core::Vector3d & sides)
{
    Shape res;

    // create nodes
    std::vector<Core::OwnedPtr<Node>> nodes;
    for(int x=0;x<=1;++x)
    {
        for(int y=0;y<=1;++y)
        {
            for(int z=0;z<=1;++z)
            {
                nodes.emplace_back(res.AddNode(x*sides[0],y*sides[1],z*sides[2]));
            }
        }
    }
    // create normals
    auto normal0 = res.AddNormal(-1, 0, 0);
    auto normal1 = res.AddNormal( 0,-1, 0);
    auto normal2 = res.AddNormal( 0, 0,-1);
    auto normal3 = res.AddNormal( 0, 0, 1);
    auto normal4 = res.AddNormal( 0, 1, 0);
    auto normal5 = res.AddNormal( 1, 0, 0);
    // add edgepairs
    auto [edge01,edge10] = res.AddEdgePair(nodes[0],nodes[1]);
    auto [edge02,edge20] = res.AddEdgePair(nodes[0],nodes[2]);
    auto [edge04,edge40] = res.AddEdgePair(nodes[0],nodes[4]);
    auto [edge05,edge50] = res.AddEdgePair(nodes[0],nodes[5]);
    auto [edge06,edge60] = res.AddEdgePair(nodes[0],nodes[6]);
    auto [edge12,edge21] = res.AddEdgePair(nodes[1],nodes[2]);
    auto [edge13,edge31] = res.AddEdgePair(nodes[1],nodes[3]);
    auto [edge15,edge51] = res.AddEdgePair(nodes[1],nodes[5]);
    auto [edge17,edge71] = res.AddEdgePair(nodes[1],nodes[7]);
    auto [edge23,edge32] = res.AddEdgePair(nodes[2],nodes[3]);
    auto [edge26,edge62] = res.AddEdgePair(nodes[2],nodes[6]);
    auto [edge27,edge72] = res.AddEdgePair(nodes[2],nodes[7]);
    auto [edge37,edge73] = res.AddEdgePair(nodes[3],nodes[7]);
    auto [edge45,edge54] = res.AddEdgePair(nodes[4],nodes[5]);
    auto [edge46,edge64] = res.AddEdgePair(nodes[4],nodes[6]);
    auto [edge47,edge74] = res.AddEdgePair(nodes[4],nodes[7]);
    auto [edge57,edge75] = res.AddEdgePair(nodes[5],nodes[7]);
    auto [edge67,edge76] = res.AddEdgePair(nodes[6],nodes[7]);
    // add faces
    auto face012 = res.AddFace(edge01,edge12,edge20,0,normal0);
    auto face026 = res.AddFace(edge02,edge26,edge60,2,normal2);
    auto face045 = res.AddFace(edge04,edge45,edge50,1,normal1);
    auto face051 = res.AddFace(edge05,edge51,edge10,1,normal1);
    auto face064 = res.AddFace(edge06,edge64,edge40,2,normal2);
    auto face132 = res.AddFace(edge13,edge32,edge21,0,normal0);
    auto face157 = res.AddFace(edge15,edge57,edge71,3,normal3);
    auto face173 = res.AddFace(edge17,edge73,edge31,3,normal3);
    auto face237 = res.AddFace(edge23,edge37,edge72,4,normal4);
    auto face276 = res.AddFace(edge27,edge76,edge62,4,normal4);
    auto face467 = res.AddFace(edge46,edge67,edge74,5,normal5);
    auto face475 = res.AddFace(edge47,edge75,edge54,5,normal5);
    // set the surface to the generated faces
    res.m_surface = {face012,face132,face051,face045,face026,face064,
                     face173,face157,face237,face276,face467,face475};
    return res;
}

Shape Shape::Construct::Cylinder(const double length, double outerRadius, double innerRadius)
{
    Shape res;

    if(innerRadius<0.0)
    {
        innerRadius *= -outerRadius;
    }
    if(innerRadius>=outerRadius)
    {
        std::swap(innerRadius,outerRadius);
        LogWarning("Inner radius should be smaller than outer radius.");
    }
    double halfCorner = acos(innerRadius/outerRadius);
    size_t sections = ceil(Core::Constants::Pi / halfCorner);
    double corner = (Core::Constants::Pi*2.0)/sections;
    // create bottom and top nodes
    std::vector<Core::OwnedPtr<Node>> bottomNodes;
    std::vector<Core::OwnedPtr<Node>> topNodes;
    bottomNodes.reserve(sections+2);
    topNodes.reserve(sections+2);
    bottomNodes.emplace_back(res.AddNode(0.0, 0.0, 0.0));
    topNodes.emplace_back(res.AddNode(0.0, 0.0, length));
    for(size_t i=0;i<sections;++i)
    {
        double alpha = corner*i;
        bottomNodes.emplace_back(res.AddNode(outerRadius*cos(alpha), outerRadius*sin(alpha), 0.0));
        topNodes.emplace_back(res.AddNode(outerRadius*cos(alpha), outerRadius*sin(alpha), length));
    }
    // create side normals
    std::vector<Core::OwnedPtr<Core::Vector3d>> normals;
    normals.reserve(sections);
    for(size_t i=0;i<sections;++i)
    {
        double alpha = corner*(i+0.5);
        normals.emplace_back(res.AddNormal(outerRadius*cos(alpha), outerRadius*sin(alpha), 0.0));
    }
    auto normalBottom = res.AddNormal(0, 0, -1);
    auto normalTop = res.AddNormal(0, 0,  1);
    // create edges
    
    return res;
}
