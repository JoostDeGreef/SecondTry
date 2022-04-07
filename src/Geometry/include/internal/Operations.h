#pragma once

namespace Operations
{
    std::vector<Polygon2D> Union(const std::vector<Polygon2D> & polygons);
    std::vector<Shape> Union(const std::vector<Shape> & shapes);
    template<typename T,typename ... Geometries>
    std::vector<T> Union(const T & geometry,const Geometries ... geometries)
    {
        return Union(std::vector<T>{{geometry, geometries...}});
    }

    // Difference();
    // Intersection();
    // RemoveSelfIntersections();
}
