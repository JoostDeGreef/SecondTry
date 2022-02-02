#include "Geometry.h"

using namespace Core;
using namespace Geometry;

bool Polygon2D::IsClockwise() const
{
    auto & a = m_southEast-1<0?m_nodes.back():m_nodes[m_southEast-1];
    auto & b = m_nodes[m_southEast];
    auto & c = m_southEast+1>=m_nodes.size()?m_nodes.front():m_nodes[m_southEast+1];
    auto A = b-a;
    auto B = c-b;
    return A[0]*B[1] - A[1]*B[2] < 0;
}

void Polygon2D::UpdateSouthEast()
{
    if(m_nodes.back()[1]<m_nodes[m_southEast][1])
    {
        m_southEast = m_nodes.size()-1;
    }
    else if(m_nodes.back()[1]>m_nodes[m_southEast][1])
    {
    }
    else // =
    {
        if(m_nodes.back()[0]<=m_nodes[m_southEast][0])
        {
            m_southEast = m_nodes.size()-1;
        }
    }
}

TriangulatedPolygon2D::TriangulatedPolygon2D(const Polygon2D & polygon2D)
{

}
