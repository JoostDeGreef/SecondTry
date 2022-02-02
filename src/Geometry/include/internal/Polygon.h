#pragma once

// class for holding the nodes which make up a 2D polygon (in the x-y plane)
class Polygon2D
{
public:
    Polygon2D()
        : m_southEast(0)
    {
        m_nodes.reserve(32);
    }

    // add nodes in the xy plane
    template<typename ... ARGS>
    void AddNode(const ARGS & ... args)
    {
        m_nodes.emplace_back(args...);
        UpdateSouthEast();
    }

    // returns true if the polygon is defined clockwise,
    // see http://www.faqs.org/faqs/graphics/algorithms-faq/
    bool IsClockwise() const;

private:
    std::vector<Core::Vector2d> m_nodes;
    size_t m_southEast;

    void UpdateSouthEast();
};

// class which will hold a triangulated polygon in x-y (z=0),
// defined by edges
class TriangulatedPolygon2D
{
public:
    TriangulatedPolygon2D(const Polygon2D & polygon2D);
private:
    Core::SmartPtrStore<Node> m_nodesStore;
    Core::SmartPtrStore<Edge> m_edgesStore;

    std::vector<Core::OwnedPtr<Edge>> m_edges;
};
