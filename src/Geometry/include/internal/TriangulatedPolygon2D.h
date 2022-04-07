#pragma once

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
