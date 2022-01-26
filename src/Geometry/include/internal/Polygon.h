#pragma once

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
        if(m_nodes.back()[1]<m_nodes[m_southEast][1])
        {
            m_southEast = m_nodes.size()-1;
        }
        else if(m_nodes.back()[1]>m_nodes[m_southEast][1])
        {
        }
        else // =
        {
            if(m_nodes.back()[0]<=m_nodes[m_southEast][0])
            {
                m_southEast = m_nodes.size()-1;
            }
        }
    }

    // returns true if the polygon is defined clockwise,
    // see http://www.faqs.org/faqs/graphics/algorithms-faq/
    bool IsClockwise() const
    {
        auto & a = m_southEast-1<0?m_nodes.back():m_nodes[m_southEast-1];
        auto & b = m_nodes[m_southEast];
        auto & c = m_southEast+1>=m_nodes.size()?m_nodes.front():m_nodes[m_southEast+1];
        auto A = b-a;
        auto B = c-b;
        return A[0]*B[1] - A[1]*B[2] < 0;
    }
private:
    std::vector<Core::Vector2d> m_nodes;
    size_t m_southEast;
};

class TriangulatedPolygon2D
{
public:
    TriangulatedPolygon2D(const Polygon2D & polygon2D)
    {

    }
private:
    Core::SmartPtrStore<Node> m_nodesStore;
    Core::SmartPtrStore<Edge> m_edgesStore;

    std::vector<Core::OwnedPtr<Edge>> m_edges;
};
