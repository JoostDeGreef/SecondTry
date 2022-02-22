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
    return A[0]*B[1] - A[1]*B[0] > 0;
}

void Polygon2D::UpdateSouthEast()
{
    const auto & A = m_nodes.back().m_vertex;
    const auto & B = m_nodes[m_southEast].m_vertex;
    if(A[1] < B[1])
    {
        m_southEast = m_nodes.size()-1;
    }
    else if(A[1] > B[1])
    {
    }
    else // =
    {
        if(A[0] < B[0])
        {
            m_southEast = m_nodes.size()-1;
        }
    }
}

TriangulatedPolygon2D::TriangulatedPolygon2D(const Polygon2D & polygon2D)
{
    CopyInputPolygonToVertices(polygon2D);
    TriangulateInputPolygon();
}

void TriangulatedPolygon2D::CopyInputPolygonToVertices(const Polygon2D & polygon2D)
{
    m_vertices.reserve(polygon2D.m_nodes.size());
    if(polygon2D.IsCounterClockwise())
    {
        for(auto iter = polygon2D.m_nodes.begin(); iter != polygon2D.m_nodes.end(); ++iter)
        {
            m_vertices.emplace_back(*iter);
        }
    }
    else
    {
        for(auto iter = polygon2D.m_nodes.rbegin(); iter != polygon2D.m_nodes.rend(); ++iter)
        {
            m_vertices.emplace_back(*iter);
        }
    }
    for(size_t i=0;i<m_vertices.size();++i)
    {
        m_vertices[i].m_index = i;
    }
}

void TriangulatedPolygon2D::InitializeVertexType()
{
    for(size_t i=0;i<m_vertices.size();++i)
    {
        size_t j = (i+1) % m_vertices.size();
        auto & node = m_vertices[i];
        auto & prev = m_vertices[(i+m_vertices.size()-1) % m_vertices.size()];
        auto & next = m_vertices[j];
        if(Vertex::Cmp()(node,prev) && Vertex::Cmp()(node,next))
        {
            // todo: check this
            node.m_type = angle(node-prev,next-node)>0 ? Vertex::Type::Start : Vertex::Type::Split;
        }
        else if (Vertex::Cmp()(prev,node) && Vertex::Cmp()(next,node))
        {
            // todo: check this
            node.m_type = angle(node-prev,next-node)>0 ? Vertex::Type::End : Vertex::Type::Merge;
        }
        Q.emplace(&node);
        auto e = m_edges.size();
        m_vertices[i].m_edges.emplace_back(e);
        auto & edge = m_edges.emplace_back(i,j);
        edge.m_next = j;
        edge.m_prev = (i+m_vertices.size()-1) % m_vertices.size();
    }
}

// find the in/out edges for vertex a, on an edge loop which also has vertex b in it;
// returns in/out edge indices (a->b, b->a)
std::tuple<size_t,size_t> TriangulatedPolygon2D::FindLoop(const size_t a, const size_t b)
{
    auto & edges = m_vertices[a].m_edges;
    for(size_t i = 1;i < edges.size();++i)
    {
        size_t j = m_edges[i].m_next;
        while(j!=i)
        {
            if(m_edges[j].m_node_a == b)
            {
                return std::make_tuple(m_edges[edges[i]].m_prev,edges[i]);
            }
            j = m_edges[j].m_next;
        }
    }
    return std::make_tuple(m_edges[edges[0]].m_prev,edges[0]);
};

// add a set of edges to m_edges, fill in all references
std::tuple<size_t,size_t> TriangulatedPolygon2D::AddEdgeInLoop(
    const size_t a, const size_t b,
    const size_t a1, const size_t a2,
    const size_t b1, const size_t b2)
{
    auto e1 = m_edges.size();
    auto e2 = e1+1;
    m_edges.emplace_back(a,b);
    m_edges.emplace_back(b,a);
    auto & edge_a1 = m_edges[a1];
    auto & edge_a2 = m_edges[a2];
    auto & edge_b1 = m_edges[b1];
    auto & edge_b2 = m_edges[b2];
    auto & edge_e1 = m_edges[e1];
    auto & edge_e2 = m_edges[e2];
    edge_e1.m_index = e1;
    edge_e2.m_index = e2;
    edge_e1.m_twin = e2;
    edge_e2.m_twin = e1;
    edge_a1.m_next = e1;
    edge_b2.m_prev = e1;
    edge_b1.m_next = e2;
    edge_a2.m_prev = e2;
    edge_e1.m_next = b2;
    edge_e1.m_prev = a1;
    edge_e2.m_next = a2;
    edge_e2.m_prev = b1;
    m_vertices[a].m_edges.emplace_back(e1);
    m_vertices[b].m_edges.emplace_back(e2);
    return std::make_tuple(e1,e2);
};

void TriangulatedPolygon2D::AddEdge(const size_t a, const size_t b)
{
    auto [a1,a2] = FindLoop(a,b);
    auto [b1,b2] = FindLoop(b,a);
    AddEdgeInLoop(a,b,a1,a2,b1,b2);
};

void TriangulatedPolygon2D::SetHelper(const size_t a, const size_t b)
{
    T.insert_or_assign(a,b);
};

// find the closest edge on the same y level as this node, to the left.
std::unordered_map<size_t, size_t>::iterator TriangulatedPolygon2D::FindLeftEdge(const size_t node)
{
    auto res = T.end();
    double dist = std::numeric_limits<double>::max();
    auto & n = m_vertices[node].m_node.m_vertex;
    for(auto iter = T.begin();iter != T.end(); ++iter)
    {
        auto & edge = m_edges[iter->first];
        auto & a = m_vertices[edge.m_node_a].m_node.m_vertex;
        auto & b = m_vertices[edge.m_node_b].m_node.m_vertex;
        if((a[1]<=n[1] && b[1]>=n[1]) || (b[1]<=n[1] && a[1]>=n[1]))
        {
            double d = (n[0]-a[0]) - (b[0]-a[0])*(n[1]-a[1])/(b[1]-a[1]);
            if(d>=0 && d<dist)
            {
                dist = d;
                res = iter;
            }
        }
    }
    return res;
};

// this only works for regular nodes!
bool TriangulatedPolygon2D::InteriorToTheLeft(const size_t node)
{
    auto & a = m_vertices[node].m_node.m_vertex;
    auto & b = m_vertices[(node+1)%m_vertices.size()].m_node.m_vertex;
    return (b[1]>=a[1]);
};

void TriangulatedPolygon2D::SplitInMonotonePieces()
{
    for(auto & Vi:Q)
    {
        switch(Vi->m_type)
        {
            case Vertex::Type::Start:
                SetHelper(Vi->m_index,Vi->m_index);
                break;
            case Vertex::Type::End:
                {
                    auto iter = T.find((Vi->m_index+m_vertices.size()-1)%m_vertices.size());
                    if(iter != T.end())
                    {
                        auto helper = iter->second;
                        if(m_vertices[helper].m_type == Vertex::Type::Merge)
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
                    assert(leftEdgeIter != T.end());
                    AddEdge(Vi->m_index,leftEdgeIter->second);
                    SetHelper(leftEdgeIter->first,Vi->m_index);
                    SetHelper(Vi->m_index,Vi->m_index);
                }
                break;
            case Vertex::Type::Merge:
                {
                    auto iter = T.find((Vi->m_index+m_vertices.size()-1)%m_vertices.size());
                    if(iter != T.end())
                    {
                        auto helper = iter->second;
                        if(m_vertices[helper].m_type == Vertex::Type::Merge)
                        {
                            AddEdge(Vi->m_index,helper);
                        }
                        T.erase(iter);
                    }
                    auto leftEdgeIter = FindLeftEdge(Vi->m_index);
                    assert(leftEdgeIter != T.end());
                    if(m_vertices[leftEdgeIter->second].m_type == Vertex::Type::Merge)
                    {
                        AddEdge(Vi->m_index,leftEdgeIter->second);
                    }
                    SetHelper(leftEdgeIter->first,Vi->m_index);
                }
                break;
            case Vertex::Type::Regular:
                if(!InteriorToTheLeft(Vi->m_index))
                {
                    auto iter = T.find((Vi->m_index+m_vertices.size()-1)%m_vertices.size());
                    if(iter != T.end())
                    {
                        auto helper = iter->second;
                        if(m_vertices[helper].m_type == Vertex::Type::Merge)
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
                    if(m_vertices[leftEdgeIter->second].m_type == Vertex::Type::Merge)
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
}

void TriangulatedPolygon2D::TriangulateInputPolygon()
{
    //***********************
    // give each node a type
    //***********************
    InitializeVertexType();
    //********************************
    // split into y-monotone polygons
    //********************************
    SplitInMonotonePieces();
    //*******************************
    // triangulate monotone polygons
    //*******************************
    std::set<size_t> newEdges; // edges added to make all polygons monotone. These are used to select polygon 'edge-loops'.
    for(size_t i=m_vertices.size();i<m_edges.size();++i)
    {
        newEdges.emplace(i);
    }
    // polygon was already monotone, not split
    if(newEdges.empty() && !m_edges.empty())
    {
        newEdges.emplace(0);
    }
    while(!newEdges.empty())
    {
        // collect all edges in this loop and remove them from newEdges
        std::unordered_map<size_t,size_t> edgeAngle; // edge, 100000*angle
        std::map<size_t,size_t> angleEdge; // 100000*angle, edge
        auto edge = *newEdges.begin();
        auto next = edge;
        do
        {
            newEdges.erase(next);
            edgeAngle.emplace(next,std::numeric_limits<size_t>::max());
            next = m_edges[next].m_next;
        } 
        while (edge != next);
        assert(edgeAngle.size()>=3);
        // calculate the angles
        if(edgeAngle.size()>3)
        {
            auto AddEdgeAngle = [&](std::pair<const size_t, size_t> &edgeAngleKey)
            {
                auto edge = edgeAngleKey.first;
                auto & v0 = m_vertices[m_edges[edge].m_node_a];
                auto & v1 = m_vertices[m_edges[edge].m_node_b];
                auto & v2 = m_vertices[m_edges[m_edges[edge].m_next].m_node_b];
                auto a = angle(v1-v0,v2-v1);
                if(a<0)
                {
                    a += Core::Constants::Pi*2;
                }
                size_t ia = 100000 * a;
                while(angleEdge.find(ia) != angleEdge.end())
                {
                    ++ia;
                }
                angleEdge.emplace(ia,edge);
                edgeAngleKey.second = ia;
            };
            for(auto & edgeAngleKey:edgeAngle)
            {
                AddEdgeAngle(edgeAngleKey);
            }
            while (edgeAngle.size()>3)
            {
                // split of the triangle with the narrowest angle
                size_t a2 = angleEdge.begin()->second;
                size_t a1 = m_edges[a2].m_prev;
                size_t a = m_edges[a2].m_node_a;
                size_t b1 = m_edges[a2].m_next;
                size_t b2 = m_edges[b1].m_next;
                size_t b = m_edges[b2].m_node_a;
                auto [e1,e2] = AddEdgeInLoop(a,b,a1,a2,b1,b2);
                assert(m_edges[m_edges[m_edges[e2].m_next].m_next].m_next == e2);
                // remove a2,b1 from the loop angle list, and e1 and update a1
                edgeAngle.erase(a2);
                edgeAngle.erase(b1);
                edgeAngle.emplace(e1,std::numeric_limits<size_t>::max());
                angleEdge.erase(edgeAngle[a1]);
                AddEdgeAngle(*edgeAngle.find(a1));
                AddEdgeAngle(*edgeAngle.find(e1));
            }
        }
    }
    for(size_t i=0;i<m_edges.size();++i)
    {
        m_edges[i].m_index = i;
    }
}
