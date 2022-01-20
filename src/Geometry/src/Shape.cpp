#include "Geometry.h"

using namespace Core;
using namespace Geometry;

Geometry::Shape::Shape()
{}

Shape::~Shape()
{
    Clear();
}

Shape::Shape(Shape && other)
{
    Swap(std::move(other));
}

Shape::Shape(const Shape & other)
{
    Copy(other);
}

Shape & Shape::Copy(const Shape & other)
{
    auto nodeMap = m_nodes.DeepCopy(other.m_nodes);
    auto edgeMap = m_edges.ShallowCopy(other.m_edges);
    auto faceMap = m_faces.ShallowCopy(other.m_faces);
    
    for(auto edges:edgeMap)
    {
        edges.second->MappedAssign(edges.first,nodeMap,edgeMap,faceMap);
    }
    for(auto faces:faceMap)
    {
        faces.second->MappedAssign(faces.first,this,edgeMap);
    }
    
    return *this;
}

Shape & Shape::Swap(Shape && other)
{
    m_nodes.Swap(other.m_nodes);
    m_edges.Swap(other.m_edges);
    m_faces.Swap(other.m_faces);
    return *this;
}

void Shape::Clear()
{
    m_nodes.Clear();
    m_edges.Clear();
    m_faces.Clear();
}

Shape Shape::Construct::Cube(const double side)
{
    return Box({side,side,side});
}

Shape Shape::Construct::Box(const Core::Vector3d & sides)
{
    Shape res;

    // create nodes
    std::vector<Node*> nodes;
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
    std::vector<Edge*> edges = {edge01,edge10,edge02,edge20,edge04,edge40,edge05,edge50,edge06,edge60,
                                edge12,edge21,edge13,edge31,edge15,edge51,edge17,edge71,
                                edge23,edge32,edge26,edge62,edge27,edge72,
                                edge37,edge73,
                                edge45,edge54,edge46,edge64,edge47,edge74,
                                edge57,edge75,
                                edge67,edge76};
    // add faces
    Face * face012 = res.AddFace(edge01,edge12,edge20,0);
    Face * face132 = res.AddFace(edge13,edge32,edge21,0);
    Face * face051 = res.AddFace(edge05,edge51,edge10,1);
    Face * face045 = res.AddFace(edge04,edge45,edge50,1);
    Face * face026 = res.AddFace(edge02,edge26,edge60,2);
    Face * face064 = res.AddFace(edge06,edge64,edge40,2);
    Face * face173 = res.AddFace(edge17,edge73,edge31,3);
    Face * face157 = res.AddFace(edge15,edge57,edge71,3);
    Face * face237 = res.AddFace(edge23,edge37,edge72,4);
    Face * face276 = res.AddFace(edge27,edge76,edge62,4);
    Face * face467 = res.AddFace(edge46,edge67,edge74,5);
    Face * face475 = res.AddFace(edge47,edge75,edge54,5);
    // set the surface to the generated faces
    res.m_surface = {face012,face132,face051,face045,face026,face064,
                     face173,face157,face237,face276,face467,face475};                                
    // reduce node refcount
    for(auto np:nodes)
    {
        res.m_nodes.Release(np);
    }
    // reduce edge refcount
    for(auto ep:edges)
    {
        res.m_edges.Release(ep);
    }
    return res;
}
