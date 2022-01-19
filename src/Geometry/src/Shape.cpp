#include "Geometry.h"

using namespace Geometry;

Shape::Shape()
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