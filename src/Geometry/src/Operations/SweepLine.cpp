#include "Geometry.h"

#include "internal/Operations/Vector2dCompare.h"
#include "internal/Operations/SweepLine.h"
#include "internal/Operations/SweepNode.h"

using namespace Core;
using namespace Geometry;
using namespace Operations;

void SweepLine::SetVertexOrder()
{
    if( !SweepNodeCompare()(m_vertices[0],m_vertices[1]) )
    {
        std::swap(m_vertices[0],m_vertices[1]);
        std::swap(m_polygons[0],m_polygons[1]);
    }
}

bool SweepLineCompare::operator () (const SweepLine & a, const SweepLine & b) const
{
    return SweepNodeCompare()(a.GetVertex(0), b.GetVertex(0));
}

bool EventLineCompare::Equal(const SweepLine & a, const SweepLine & b)
{
    return Core::Numerics::Equal(a.GetVertex(0).GetVertex()[0], b.GetVertex(0).GetVertex()[0]);
}

bool EventLineCompare::operator () (const SweepLine & a, const SweepLine & b) const
{
    return EventNodeCompare()(a.GetVertex(0), b.GetVertex(0));
}

SweepLine::Intersection SweepLine::DetermineIntersection(const SweepLine & other) const
{
    auto slope0 = m_vertices[0]->GetVertex() - m_vertices[1]->GetVertex();
    auto slope1 = other.m_vertices[0]->GetVertex() - other.m_vertices[1]->GetVertex();
    double den = slope0[0] * slope1[1] - slope0[1] * slope1[0];
    if (den > eps || -den > eps)
    {
        auto pivot = m_vertices[0]->GetVertex() - other.m_vertices[0]->GetVertex();
        double s = (pivot[0] * slope1[1] - pivot[1] * slope1[0])/den;
        double t = (pivot[0] * slope0[1] - pivot[1] * slope0[0])/den;
        Intersection::Type it = Intersection::Type::None;
        if( s+eps>0 && s-eps<1 && t+eps>0 && t-eps<1)
        {
            it=Intersection::Type::Cross;
        }
        return Intersection(it, s, t);
    }
    else
    {
        // Todo: same slope -> parallel lines, do they overlap?
        Intersection::Type it = Intersection::Type::None;
        double s = -1;
        double t = -1;
        return Intersection(it, s, t);
    }
}
