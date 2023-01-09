#include "Geometry.h"

using namespace Core;
using namespace Geometry;

double DistancePlaneToPoint(const Core::Vector3d & planeNormal,const Core::Vector3d & pointOnPlane,const Core::Vector3d & point)
{
    return fabs(planeNormal.InnerProduct(pointOnPlane-point));
}

