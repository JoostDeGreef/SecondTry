#include "Core.h"
#include "Geometry.h"

#include "internal/Operations/Vector2dCompare.h"
#include "internal/Operations/SweepLine.h"
#include "internal/Operations/SweepNode.h"
#include "internal/Operations/Sweeper.h"
#include "internal/Operations/Boolean.h"

using namespace Core;
using namespace Geometry;
using namespace Operations;

std::vector<Polygon2D> Operations::Union(const std::vector<Polygon2D> & polygons)
{
    Boolean b(polygons);
    return b.Union();
}

