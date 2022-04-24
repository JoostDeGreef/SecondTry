#pragma once

namespace Geometry
{
namespace Operations
{
    static constexpr double eps = 1e-6;

    inline
    bool operator < (const Core::Vector2d & a,const Core::Vector2d & b)
    {
        return a[1] + eps < b[1] ? true
             : b[1] < a[1] + eps ? false
             : a[0] + eps < b[0] ? true
             :                     false;
    }
    inline
    bool operator > (const Core::Vector2d & a,const Core::Vector2d & b)
    {
        return a[1] > b[1] + eps ? true
             : b[1] + eps > a[1] ? false
             : a[0] > b[0] + eps ? true
             :                     false;
    }
    inline
    bool operator == (const Core::Vector2d & a,const Core::Vector2d & b)
    {
        return a[1] > b[1] + eps ? false
             : a[1] + eps < b[1] ? false
             : a[0] > b[0] + eps ? false
             : a[0] + eps < b[0] ? false
             :                     true;
    }
    inline
    bool operator != (const Core::Vector2d & a,const Core::Vector2d & b)
    {
        return a[1] > b[1] + eps ? true
             : a[1] + eps < b[1] ? true
             : a[0] > b[0] + eps ? true
             : a[0] + eps < b[0] ? true
             :                     false;
    }
}
}