#pragma once

namespace Geometry
{
namespace Operations
{
    static constexpr double eps = 1e-6;

    inline 
    bool Less(const double & a, const double b)
    {
        return Core::Numerics::Less(a,b,eps);
    }

    inline 
    bool LessOrEqual(const double & a, const double b)
    {
        return Core::Numerics::LessOrEqual(a,b,eps);
    }

    inline 
    bool Greater(const double & a, const double b)
    {
        return Core::Numerics::Greater(a,b,eps);
    }

    inline 
    bool GreaterOrEqual(const double & a, const double b)
    {
        return Core::Numerics::GreaterOrEqual(a,b,eps);
    }

    inline 
    bool Equal(const double & a, const double b)
    {
        return Core::Numerics::Equal(a,b,eps);
    }

    inline 
    bool NotEqual(const double & a, const double b)
    {
        return Core::Numerics::NotEqual(a,b,eps);
    }


    // Note: Y-dominant compares!
    inline
    bool operator < (const Core::Vector2d & a,const Core::Vector2d & b)
    {
        return Less(a[1],b[1]) || (LessOrEqual(a[1],b[1]) && Less(a[0],b[0]));
    }
    inline
    bool operator > (const Core::Vector2d & a,const Core::Vector2d & b)
    {
        return Greater(a[1],b[1]) || (GreaterOrEqual(a[1],b[1]) && Greater(a[0],b[0]));
    }
    inline
    bool operator == (const Core::Vector2d & a,const Core::Vector2d & b)
    {
        return Equal(a[1],b[1]) && Equal(a[0],b[0]);
    }
    inline
    bool operator != (const Core::Vector2d & a,const Core::Vector2d & b)
    {
        return NotEqual(a[1],b[1]) && NotEqual(a[0],b[0]);
    }
}
}