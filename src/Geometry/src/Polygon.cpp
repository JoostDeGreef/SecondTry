#include "Geometry.h"

using namespace Core;
using namespace Geometry;

bool Polygon2D::IsCounterClockwise() const
{
    auto & a = m_southEast-1<0?m_nodes.back().m_vertex:m_nodes[m_southEast-1].m_vertex;
    auto & b = m_nodes[m_southEast].m_vertex;
    auto & c = m_southEast+1>=m_nodes.size()?m_nodes.front().m_vertex:m_nodes[m_southEast+1].m_vertex;
    auto A = b - a;
    auto B = c - b;
    return A[0]*B[1] - A[1]*B[2] > 0;
}

void Polygon2D::UpdateSouthEast()
{
    const auto & A = m_nodes.back().m_vertex;
    const auto & B = m_nodes[m_southEast].m_vertex;
    if(A[1]< B[1])
    {
        m_southEast = m_nodes.size()-1;
    }
    else if(A[1] > B[1])
    {
    }
    else // =
    {
        if(A[0] <= B[0])
        {
            m_southEast = m_nodes.size()-1;
        }
    }
}

TriangulatedPolygon2D::TriangulatedPolygon2D(const Polygon2D & polygon2D)
{
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

        Polygon2D::Node m_node;
        Type m_type;
        size_t m_index;
    };
    std::vector<Vertex> vertices;
    vertices.reserve(polygon2D.m_nodes.size());
    if(polygon2D.IsCounterClockwise())
    {
        for(auto iter = polygon2D.m_nodes.begin(); iter != polygon2D.m_nodes.end(); ++iter)
        {
            vertices.emplace_back(*iter);
        }
    }
    else
    {
        for(auto iter = polygon2D.m_nodes.rbegin(); iter != polygon2D.m_nodes.rend(); ++iter)
        {
            vertices.emplace_back(*iter);
        }
    }
    for(size_t i=0;i<vertices.size();++i)
    {
        vertices[i].m_index = i;
    }
    // helper for sorting vertices
    struct VertexCmp
    {
        bool operator()(const Vertex * a,
                        const Vertex * b) const
        {
            return VertexCmp()(*a,*b);
        }
        bool operator()(const Vertex & a,
                        const Vertex & b) const
        {
            return VertexCmp()(a.m_node.m_vertex,b.m_node.m_vertex);
        }
        bool operator()(const Vector2d & a,
                        const Vector2d & b) const
        {
            const auto & ax = a[0];
            const auto & ay = a[1];
            const auto & bx = b[0];
            const auto & by = b[1];
            if(ay < by) return true;
            if(ay > by) return false;
            if(ax < bx) return true;
            return false;
        }
    };
    // calculate angle between two vectors
    auto angle = [](const Core::Vector2d & a,const Core::Vector2d & b)
    {
        auto dot = a[0]*b[0] + a[1]*b[1];
        auto det = a[0]*b[1] - a[1]*b[0];
        return atan2(det, dot);
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
    };
    std::vector<Edge> edges; // predefined edges
    std::set<Vertex*,VertexCmp> Q; // vertices sorted by y
    // give each node a type
    for(size_t i=0;i<vertices.size();++i)
    {
        size_t j = (i+1) % vertices.size();
        auto & node = vertices[i];
        auto & prev = vertices[(i+vertices.size()-1) % vertices.size()];
        auto & next = vertices[j];
        if(VertexCmp()(node,prev) && VertexCmp()(node,next))
        {
            // todo: check this
            node.m_type = angle(node-prev,next-node)<0 ? Vertex::Type::Start : Vertex::Type::Split;
        }
        else if (VertexCmp()(prev,node) && VertexCmp()(next,node))
        {
            // todo: check this
            node.m_type = angle(node-prev,next-node)<0 ? Vertex::Type::End : Vertex::Type::Merge;
        }
        Q.emplace(&node);
        edges.emplace_back(i,j);
    }
    // split into y-monotone polygons
    std::unordered_map<size_t,size_t> T; // edge, helper vertex
    auto AddEdge = [&](const size_t a, const size_t b)
    {
        edges.emplace_back(a,b);
    };
    auto SetHelper = [&](const size_t a, const size_t b)
    {
        T.emplace(a,b);
    };
    auto FindLeftEdge = [&](const size_t node)
    {
        //todo
        return T.begin();
    };
    auto InteriorToTheLeft = [&](const size_t node)
    {
        // todo
        return true;
    };
    for(auto & Vi:Q)
    {
        switch(Vi->m_type)
        {
            case Vertex::Type::Start:
                SetHelper(Vi->m_index,Vi->m_index);
                break;
            case Vertex::Type::End:
                {
                    auto iter = T.find(Vi->m_index-1);
                    if(iter != T.end())
                    {
                        auto helper = iter->second;
                        if(vertices[helper].m_type == Vertex::Type::Merge)
                        {
                            AddEdge(Vi->m_index,helper);
                        }
                        T.erase(iter);
                    }
                }
                break;
            case Vertex::Type::Split:
                {
                    auto leftEdgeIter = FindLeftEdge(Vi->m_index);
                    AddEdge(Vi->m_index,leftEdgeIter->second);
                    SetHelper(leftEdgeIter->first,Vi->m_index);
                    SetHelper(Vi->m_index,Vi->m_index);
                }
                break;
            case Vertex::Type::Merge:
                {
                    auto iter = T.find(Vi->m_index-1);
                    if(iter != T.end())
                    {
                        auto helper = iter->second;
                        if(vertices[helper].m_type == Vertex::Type::Merge)
                        {
                            AddEdge(Vi->m_index,helper);
                        }
                        T.erase(iter);
                    }
                    auto leftEdgeIter = FindLeftEdge(Vi->m_index);
                    if(vertices[leftEdgeIter->second].m_type == Vertex::Type::Merge)
                    {
                        AddEdge(Vi->m_index,leftEdgeIter->second);
                    }
                    SetHelper(leftEdgeIter->first,Vi->m_index);
                }
                break;
            case Vertex::Type::Regular:
                if(InteriorToTheLeft(Vi->m_index))
                {
                    auto iter = T.find(Vi->m_index-1);
                    if(iter != T.end())
                    {
                        auto helper = iter->second;
                        if(vertices[helper].m_type == Vertex::Type::Merge)
                        {
                            AddEdge(Vi->m_index,helper);
                        }
                        T.erase(iter);
                    }
                    SetHelper(Vi->m_index,Vi->m_index);
                }
                else
                {
                    auto leftEdgeIter = FindLeftEdge(Vi->m_index);
                    if(vertices[leftEdgeIter->second].m_type == Vertex::Type::Merge)
                    {
                        AddEdge(Vi->m_index,leftEdgeIter->second);
                    }
                    SetHelper(leftEdgeIter->first,Vi->m_index);
                }
                break;
            default:
                // Something went wrong
                break;
        }
    }
    // TODO
    //  - triangulate monotone polygons
    //  - create the 3D structures
}
