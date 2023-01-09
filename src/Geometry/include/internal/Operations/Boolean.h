#pragma once

namespace Geometry
{
namespace Operations
{
    class Boolean
    {
    public:
        Boolean(const std::vector<Polygon2D> & polygons)
            : m_sweeper(polygons)
        {}

        std::vector<Polygon2D> Union()
        {
            std::vector<Polygon2D> res;
            return res;
        }   

        std::vector<Polygon2D> Difference()
        {
            std::vector<Polygon2D> res;
            return res;
        }

        std::vector<Polygon2D>  Intersection()
        {
            std::vector<Polygon2D> res;
            return res;
        }
    private:
        Sweeper m_sweeper;
    };
}
}
