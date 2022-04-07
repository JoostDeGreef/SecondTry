#include "Geometry.h"

using namespace Core;
using namespace Geometry;

bool Polygon2D::IsCounterClockwise() const
{
    auto & a = m_southEast<1?m_nodes.back().m_vertex:m_nodes[m_southEast-1].m_vertex;
    auto & b = m_nodes[m_southEast].m_vertex;
    auto & c = m_southEast+1>=m_nodes.size()?m_nodes.front().m_vertex:m_nodes[m_southEast+1].m_vertex;
    auto A = b - a;
    auto B = c - b;
    return A[0]*B[1] - A[1]*B[0] > 0;
}

void Polygon2D::UpdateSouthEast()
{
    if(m_nodes.size()<2)
    {
        m_southEast = 0;
    }
    else
    {
        const auto & A = m_nodes.back().m_vertex;
        const auto & B = m_nodes[m_southEast].m_vertex;
        if(A[1] < B[1])
        {
            m_southEast = m_nodes.size()-1;
        }
        else if(A[1] > B[1])
        {
        }
        else // =
        {
            if(A[0] < B[0])
            {
                m_southEast = m_nodes.size()-1;
            }
        }
    }
}

double Polygon2D::Surface() const
{
    double surface = 0;
    auto start = m_nodes.back();
    for(const auto & end:m_nodes)
    {
        surface += start.m_vertex[0]*end.m_vertex[1]-start.m_vertex[1]*end.m_vertex[0];
    }
    return fabs(0.5 * surface);
}

