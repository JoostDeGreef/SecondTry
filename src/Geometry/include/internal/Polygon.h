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
    public:
        enum class Normal
        {
            None,
            Single,
            Both,
            In,
            Out
        };

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

        Core::Vector2d m_vertex;
        Core::Vector2d m_normal1;
        Core::Vector2d m_normal2;
        Normal m_normal;
    };

    Polygon2D()
        : m_southEast(0)
    {
        m_nodes.reserve(32);
    }

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
    bool IsCounterClockwise() const;

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
    class Vertex
    {
    public:
        enum class Type
        {
            Start,
            End,
            Regular,
            Split,
            Merge
        };
        Vertex(const Polygon2D::Node & node)
            : m_node(node)
            , m_type(Type::Regular)
        {}
        Core::Vector2d operator - (const Vertex & other)
        {
            return m_node.m_vertex - other.m_node.m_vertex;
        }

        Polygon2D::Node m_node; // node, includes normal information
        std::vector<size_t> m_edges; // all edges with this start vertex
        Type m_type;
        size_t m_index;

        // helper for sorting vertices
        struct Cmp
        {
            bool operator()(const Vertex * a,
                            const Vertex * b) const
            {
                return Cmp()(*a,*b);
            }
            bool operator()(const Vertex & a,
                            const Vertex & b) const
            {
                return Cmp()(a.m_node.m_vertex,b.m_node.m_vertex);
            }
            bool operator()(const Core::Vector2d & a,
                            const Core::Vector2d & b) const
            {
                const auto & ax = a[0];
                const auto & ay = a[1];
                const auto & bx = b[0];
                const auto & by = b[1];
                if(ay > by) return true;
                if(ay < by) return false;
                if(ax < bx) return true;
                return false;
            }
        };
    };

    // edges
    class Edge
    {
    public:
        Edge(const size_t node_a,const size_t node_b)
            : m_node_a(node_a)
            , m_node_b(node_b)
        {}

        size_t m_node_a;
        size_t m_node_b;

        size_t m_prev = -1;
        size_t m_next = -1;
        size_t m_twin = -1;

        size_t m_index = -1;
    };

    TriangulatedPolygon2D(const Polygon2D & polygon2D);

    const auto & GetVertices() const { return m_vertices; }
    const auto & GetEdges() const { return m_edges; }
protected:
    // available as output
    std::vector<Vertex> m_vertices;
    std::vector<Edge> m_edges;
    // for internal use
    std::set<Vertex*,Vertex::Cmp> Q; // vertices sorted by y
    std::unordered_map<size_t,size_t> T; // edge, helper vertex

    // Copy the input polygont to the internal vertices in the right order
    void CopyInputPolygonToVertices(const Polygon2D & polygon2D);

    // Triangulate the list of vertices
    void TriangulateInputPolygon();

    // Initialize the vertex type
    void InitializeVertexType();

    // Add an edge between a and b, using the provided in/out edges
    std::tuple<size_t,size_t> AddEdgeInLoop(
                             const size_t a, const size_t b,
                             const size_t a1, const size_t a2,
                             const size_t b1, const size_t b2);

    // find the in/out edges for vertex a, on an edge loop which also has vertex b in it;
    std::tuple<size_t,size_t> FindLoop(const size_t a, const size_t b);

    // add edge between vertices a and b
    void AddEdge(const size_t a, const size_t b);

    // Set the 'helper' vertex b for edge a
    void SetHelper(const size_t a, const size_t b);

    // find the closest edge on the same y level as this node, to the left.
    std::unordered_map<size_t, size_t>::iterator FindLeftEdge(const size_t node);

    // Indicate if the polygon is to the left of a regular(!) vertex
    bool InteriorToTheLeft(const size_t node);

    // Split the vertex loop in monotone pieces (Y-monotone for now)
    void SplitInMonotonePieces();

    // calculate angle between two vectors
    static double angle(const Core::Vector2d & a,const Core::Vector2d & b)
    {
        auto dot = a[0]*b[0] + a[1]*b[1];
        auto det = a[0]*b[1] - a[1]*b[0];
        return atan2(det, dot);
    };
};
