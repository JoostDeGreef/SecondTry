#pragma once

class TriangulatedPolygon2D;

// class for holding the nodes which make up a 2D polygon (in the x-y plane)
class Polygon2D
{
    friend class TriangulatedPolygon2D;
public:
    class Node
    {
        friend class Polygon2D;
        friend class TriangulatedPolygon2D;
        enum class Normal
        {
            None,
            Single,
            Both,
            In,
            Out
        };
    public:
        Node(const Core::Vector2d &vertex)
            : m_vertex(vertex)
            , m_normal(Normal::None)
        {}
        Node(const Core::Vector2d &vertex,
             const Core::Vector2d &normal)
            : m_vertex(vertex)
            , m_normal1(normal)
            , m_normal(Normal::Single)
        {}
        Node(const Core::Vector2d &vertex,
             const Core::Vector2d &normal1,
             const Core::Vector2d &normal2)
            : m_vertex(vertex)
            , m_normal1(normal1)
            , m_normal2(normal2)
            , m_normal(Normal::Both)
        {}
        Node(const Core::Vector2d &vertex,
             const Core::Vector2d &normal,
             const bool inNormal)
            : m_vertex(vertex)
            , m_normal1(normal)
            , m_normal2(normal)
            , m_normal(inNormal?Normal::In:Normal::Out)
        {}

    private:
        Core::Vector2d m_vertex;
        Core::Vector2d m_normal1;
        Core::Vector2d m_normal2;
        Normal m_normal;
        int m_userdata = -1;
    };

    Polygon2D()
        : m_southEast(0)
    {
        m_nodes.reserve(32);
    }
    Polygon2D(const Polygon2D & polygon2D);

    // add nodes in the xy plane
    // if an edge has no normals defined at the vertices, the face normal will be used for the side faces.
    void AddNode(const Core::Vector2d &vertex)
    {
        m_nodes.emplace_back(vertex);
        UpdateSouthEast();
    }
    void AddNode(const Core::Vector2d &vertex,
                 const Core::Vector2d &normal)
    {
        m_nodes.emplace_back(vertex,normal);
        UpdateSouthEast();
    }
    void AddNode(const Core::Vector2d &vertex,
                 const Core::Vector2d &normal1,
                 const Core::Vector2d &normal2)
    {
        m_nodes.emplace_back(vertex,normal1,normal2);
        UpdateSouthEast();
    }
    void AddNode(const Core::Vector2d &vertex,
                 const Core::Vector2d &normal,
                 const bool inNormal)
    {
        m_nodes.emplace_back(vertex,normal,inNormal);
        UpdateSouthEast();
    }

    // returns true if the polygon is defined clockwise,
    // see http://www.faqs.org/faqs/graphics/algorithms-faq/
    bool IsClockwise() const;

private:
    std::vector<Node> m_nodes;
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
    Polygon2D m_polygon2D;

    Core::SmartPtrStore<Node> m_nodesStore;
    Core::SmartPtrStore<Edge> m_edgesStore;
    Core::SmartPtrStore<Core::Vector3d> m_normalsStore;

    std::vector<Core::OwnedPtr<Edge>> m_edges;
};
