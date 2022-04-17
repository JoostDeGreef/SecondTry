#include "Geometry.h"

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
