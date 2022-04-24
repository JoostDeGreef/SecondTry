#include "Geometry.h"

#include "internal/Operations/Vector2dCompare.h"
#include "internal/Operations/SweepLine.h"
#include "internal/Operations/SweepNode.h"

using namespace Core;
using namespace Geometry;
using namespace Operations;

void SweepLine::DetermineBottomLeft()
{
    if( SweepNodeCompare()(m_vertices[0],m_vertices[1]) )
    {
        m_vertices[2] = m_vertices[0];
    }
    else
    {
        m_vertices[2] = m_vertices[1];
    }
}

bool SweepLineCompare::operator () (const SweepLine & a, const SweepLine & b) const
{
    return SweepNodeCompare()(a.GetBottomLeftVertex(), b.GetBottomLeftVertex());
}

SweepLine::Intersection SweepLine::DetermineIntersection(const SweepLine & other) const
{
    auto slope0 = m_vertices[1]->GetVertex() - m_vertices[0]->GetVertex();
    auto slope1 = other.m_vertices[1]->GetVertex() - other.m_vertices[0]->GetVertex();
    double den = slope0[0] * slope1[1] - slope0[1] * slope1[0];
    if (den > eps || -den > eps)
    {
        auto pivot = m_vertices[0]->GetVertex() - other.m_vertices[0]->GetVertex();
        double s = (pivot[1] * slope0[0] - pivot[0] * slope0[1])/den;
        double t = (pivot[1] * slope1[0] - pivot[0] * slope1[1])/den;
        Intersection::Type it = Intersection::Type::None;
        if( s+eps>0 && s-eps<1 && t+eps>0 && t-eps<1)
        {
            it=Intersection::Type::Cross;
        }
        return Intersection(it, s, t);
    }
    else
    {
        // Todo:
        Intersection::Type it = Intersection::Type::None;
        double s = 0;
        double t = 0;
        return Intersection(it, s, t);
    }
}
