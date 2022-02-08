#include "Geometry.h"

using namespace Core;
using namespace Geometry;

Polygon2D::Polygon2D(const Polygon2D & other)
{
    if(other.IsClockwise())
    {
        m_nodes = other.m_nodes;
        m_southEast = other.m_southEast;
    }
    else
    {
        m_nodes.reserve(other.m_nodes.size());
        for(auto iter = other.m_nodes.rbegin(); iter != other.m_nodes.rend(); ++iter)
        {
            m_nodes.emplace_back(*iter);
        }
        m_southEast = other.m_nodes.size()-other.m_southEast-1;
    }
}

bool Polygon2D::IsClockwise() const
{
    auto & a = m_southEast-1<0?m_nodes.back().m_vertex:m_nodes[m_southEast-1].m_vertex;
    auto & b = m_nodes[m_southEast].m_vertex;
    auto & c = m_southEast+1>=m_nodes.size()?m_nodes.front().m_vertex:m_nodes[m_southEast+1].m_vertex;
    auto A = b - a;
    auto B = c - b;
    return A[0]*B[1] - A[1]*B[2] < 0;
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
    : m_polygon2D(polygon2D)
{
    // helper for sorting nodes
    struct NodeCmp
    {
        bool operator()(const Polygon2D::Node* a,const Polygon2D::Node* b) const
        {
            const auto & ax = a->m_vertex[0];
            const auto & ay = a->m_vertex[1];
            const auto & bx = b->m_vertex[0];
            const auto & by = b->m_vertex[1];
            if(ay < by) return true;
            if(ay > by) return false;
            if(ax < bx) return true;
            return false;
        }
        bool operator()(const Node & a,const Node & b) const
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
    auto angle = [](const Core::Vector3d & a,const Core::Vector3d & b)
    {
        auto dot = a[0]*b[0] + a[1]*b[1];
        auto det = a[0]*b[1] - a[1]*b[0];
        return atan2(det, dot);
    };

    // create 3D vertices in the x-y plane
    for(size_t i=0;i<polygon2D.m_nodes.size();++i)
    {
        m_polygon2D.m_nodes[i].m_userdata = m_edges.size();
        const auto & vertex = m_polygon2D.m_nodes[i].m_vertex; 
        m_edges.emplace_back();
        auto & edge = m_edges.back();
        edge->SetStart(m_nodesStore.Create(vertex[0],vertex[1],0.0));
    }
    // set edge next/prev
    size_t i0 = m_edges.size()-1;
    for(size_t i1=0;i1<m_edges.size();++i1)
    {
        size_t i2 = (i1 + 1) % m_edges.size();
        m_edges[i1]->SetNext(m_edges[i2]);
        m_edges[i1]->SetPrev(m_edges[i0]);
        i0 = i1;
    }
    // give each node a type
    // todo: combine this with the 'set edge next/prev' loop?
    enum class NodeType
    {
        Start,
        End,
        Regular,
        Split,
        Merge
    };
    std::vector<NodeType> nodeTypes;
    nodeTypes.resize(m_edges.size());
    for(size_t i=0;i<m_edges.size();++i)
    {
        const auto & edge = m_edges[i];
        const auto & node = *edge->Start();
        const auto & prev = *edge->Prev()->Start();
        const auto & next = *edge->Next()->Start();
        if(NodeCmp()(node,prev) && NodeCmp()(node,next))
        {
            // todo: check this
            nodeTypes[i] = angle(node-prev,next-node)<0 ? NodeType::Start : NodeType::Split;
        }
        else if (NodeCmp()(prev,node) && NodeCmp()(next,node))
        {
            // todo: check this
            nodeTypes[i] = angle(node-prev,next-node)<0 ? NodeType::End : NodeType::Merge;
        }
        else
        {
            nodeTypes[i] = NodeType::Regular;
        }
    }
    // split into y-monotone polygons
    std::set<Polygon2D::Node*,NodeCmp> Q;
    for(auto & node:m_polygon2D.m_nodes)
    {
        Q.emplace(&node);
    }
    for(auto * node:Q)
    {
        // todo: handle node by type
    }
    // TODO
    //  - split into monotone polygons
    //  - triangulate monotone polygons
}
