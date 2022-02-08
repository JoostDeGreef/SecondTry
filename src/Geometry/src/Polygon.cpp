#include "Geometry.h"

using namespace Core;
using namespace Geometry;

Polygon2D::Polygon2D(const Polygon2D & other)
{
    if(other.IsClockwise())
    {
        m_nodes = other.m_nodes;
        m_southEast = other.m_southEast;
    }
    else
    {
        m_nodes.reserve(other.m_nodes.size());
        for(auto iter = other.m_nodes.rbegin(); iter != other.m_nodes.rend(); ++iter)
        {
            m_nodes.emplace_back(*iter);
        }
        m_southEast = other.m_nodes.size()-other.m_southEast-1;
    }
}

bool Polygon2D::IsClockwise() const
{
    auto & a = m_southEast-1<0?m_nodes.back().m_vertex:m_nodes[m_southEast-1].m_vertex;
    auto & b = m_nodes[m_southEast].m_vertex;
    auto & c = m_southEast+1>=m_nodes.size()?m_nodes.front().m_vertex:m_nodes[m_southEast+1].m_vertex;
    auto A = b - a;
    auto B = c - b;
    return A[0]*B[1] - A[1]*B[2] < 0;
}

void Polygon2D::UpdateSouthEast()
{
    const auto & A = m_nodes.back().m_vertex;
    const auto & B = m_nodes[m_southEast].m_vertex;
    if(A[1]< B[1])
    {
        m_southEast = m_nodes.size()-1;
    }
    else if(A[1] > B[1])
    {
    }
    else // =
    {
        if(A[0] <= B[0])
        {
            m_southEast = m_nodes.size()-1;
        }
    }
}

TriangulatedPolygon2D::TriangulatedPolygon2D(const Polygon2D & polygon2D)
    : m_polygon2D(polygon2D)
{
    // create 3D vertices in the x-y plane
    for(size_t i=0;i<polygon2D.m_nodes.size();++i)
    {
        const auto & vertex = polygon2D.m_nodes[i].m_vertex; 
        m_edges.emplace_back();
        auto & edge = m_edges.back();
        edge->SetStart(m_nodesStore.Create(vertex[0],vertex[1],0.0));
    }
    // set edge next/prev
    size_t i0 = m_edges.size()-1;
    for(size_t i1=0;i1<m_edges.size();++i1)
    {
        size_t i2 = (i1 + 1) % m_edges.size();
        m_edges[i1]->SetNext(m_edges[i2]);
        m_edges[i1]->SetPrev(m_edges[i0]);
        i0 = i1;
    }
    // TODO
    //  - split into monotone polygons
    //  - triangulate monotone polygons
}
