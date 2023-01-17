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
    char* raw = new char[m_surface.size()*sizeof(Core::OwnedPtr<Geometry::Face>)];
    Core::OwnedPtr<Geometry::Face>* rawFaces = new (raw) Core::OwnedPtr<Geometry::Face>[m_surface.size()];
    for (size_t i=0;i<m_surface.size();++i)
    {
        rawFaces[i] = m_surface[i];
    }
    m_surface.clear();
    delete raw;
    m_normals.ClearWithoutCallingDestructors();
    m_nodes.ClearWithoutCallingDestructors();
    m_edges.ClearWithoutCallingDestructors();
    m_faces.ClearWithoutCallingDestructors();
    UpdateLastChangeID();
}

std::tuple<Core::OwnedPtr<Geometry::Edge>,Core::OwnedPtr<Geometry::Edge>> Shape::AddEdgePair(
    const Core::OwnedPtr<Node> & n0,
    const Core::OwnedPtr<Node> & n1)
{
    auto edge0 = AddEdge();
    auto edge1 = AddEdge();
    edge0->SetTwin(edge1);
    edge1->SetTwin(edge0);
    edge0->SetStart(n0);
    edge1->SetStart(n1);
    return std::make_tuple(edge0,edge1);
}

Core::OwnedPtr<Face> Shape::AddFace(
    Core::OwnedPtr<Edge> & e0,
    Core::OwnedPtr<Edge> & e1,
    Core::OwnedPtr<Edge> & e2,
    uint64_t facegroup,
    Core::OwnedPtr<Core::Vector3d> normal)
{
    auto face = m_faces.Create();
    face->SetShape(this);
    face->SetEdges({e0,e1,e2});
    face->SetFacegroup(facegroup);
    e0->SetFace(face);
    e1->SetFace(face);
    e2->SetFace(face);
    e0->SetNext(e1);
    e1->SetNext(e2);
    e2->SetNext(e0);
    e0->SetPrev(e2);
    e1->SetPrev(e0);
    e2->SetPrev(e1);
    if(normal.IsSet())
    {
        face->SetNormal(normal);
    }
    else
    {
        face->CalcNormal();
    }
    return face;
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

double Shape::CalculateSurface() const
{
    double res = 0;
    for(auto face:m_surface)
    {
        res += face->CalcSurface();
    }
    return res;
}

Shape & Shape::Translate(const Core::Vector3d & translation)
{
    for(auto & node:m_nodes)
    {
        node += translation;
    }
    UpdateLastChangeID();
    return *this;
}

Shape & Shape::Rotate(const Core::Quat & rotation,const Core::Vector3d & center)
{
    for(auto & node:m_nodes)
    {
        node = center+rotation.Transform(node-center);
    }
    UpdateLastChangeID();
    return *this;
}

std::vector<float> Shape::Draw() const
{
    std::vector<float> res;
    res.reserve(m_surface.size()*3);
    for(auto & face:m_surface)
    {
        for(int i=0;i<3;++i)
        {
            auto & n = *face->GetEdge(i)->Start();
            res.push_back(n[0]);
            res.push_back(n[1]);
            res.push_back(n[2]);
        }
    }
    return res;
}

std::vector<float> Shape::DrawWithNormals() 
{
    std::vector<float> res;
    res.reserve(m_surface.size()*3*3*2);
    for(auto & face:m_surface)
    {
        for(int i=0;i<3;++i)
        {
            auto & v = *face->GetEdge(i)->Start();
            auto & n = face->GetVertexNormal(i);
            res.push_back(v[0]);
            res.push_back(v[1]);
            res.push_back(v[2]);
            res.push_back(n[0]);
            res.push_back(n[1]);
            res.push_back(n[2]);
        }
    }
    return res;
}

