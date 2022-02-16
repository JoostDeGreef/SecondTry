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
    TriangulateInputPolygon();
}

void TriangulatedPolygon2D::TriangulateInputPolygon()
{
    std::set<Vertex*,Vertex::Cmp> Q; // vertices sorted by y
    //***********************
    // give each node a type
    //***********************
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
    //********************************
    // split into y-monotone polygons
    //********************************
    std::unordered_map<size_t,size_t> T; // edge, helper vertex
    // find the in/out edges for a, on an edge loop which also has b in it;
    auto FindLoop = [&](const size_t a, const size_t b)
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
    auto AddEdgeInLoop = [&](const size_t a, const size_t b,
                             const size_t a1, const size_t a2,
                             const size_t b1, const size_t b2)
    {
        auto & edge_a1 = m_edges[a1];
        auto & edge_a2 = m_edges[a2];
        auto & edge_b1 = m_edges[b1];
        auto & edge_b2 = m_edges[b2];
        auto e1 = m_edges.size();
        auto e2 = e1+1;
        auto & edge_e1 = m_edges.emplace_back(a,b);
        auto & edge_e2 = m_edges.emplace_back(b,a);
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
    auto AddEdge = [&](const size_t a, const size_t b)
    {
        auto [a1,a2] = FindLoop(a,b);
        auto [b1,b2] = FindLoop(b,a);
        AddEdgeInLoop(a,b,a1,a2,b1,b2);
    };
    auto SetHelper = [&](const size_t a, const size_t b)
    {
        T.emplace(a,b);
    };
    // find the closest edge on the same y level as this node, to the left.
    auto FindLeftEdge = [&](const size_t node)
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
    auto InteriorToTheLeft = [&](const size_t node)
    {
        auto & a = m_vertices[node].m_node.m_vertex;
        auto & b = m_vertices[(node+1)%m_vertices.size()].m_node.m_vertex;
        return (b[1]>=a[1]);
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
                        if(m_vertices[helper].m_type == Vertex::Type::Merge)
                        {
                            AddEdge(Vi->m_index,helper);
                        }
                        T.erase(iter);
                    }
                    auto leftEdgeIter = FindLeftEdge(Vi->m_index);
                    if(m_vertices[leftEdgeIter->second].m_type == Vertex::Type::Merge)
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
    //*******************************
    // triangulate monotone polygons
    //*******************************
    std::set<size_t> newEdges; // edges added to make all polygons monotone. These are used to select polygon 'edge-loops'.
    for(size_t i=m_vertices.size();i<m_edges.size();++i)
    {
        newEdges.emplace(i);
    }
    // polygon was already monotone, not split
    if(newEdges.empty())
    {
        newEdges.emplace(0);
    }
    while(!newEdges.empty())
    {
        // find left and right side of the loop
        auto edge = *newEdges.begin();
        size_t topLeft = edge;
        size_t bottomRight = edge;
        size_t c = m_edges[edge].m_next;
        while(c != edge )
        {
            auto & e = m_edges[c];
            auto & tl = m_edges[topLeft];
            auto & e_a = m_vertices[e.m_node_a].m_node.m_vertex;
            auto & tl_a = m_vertices[tl.m_node_a].m_node.m_vertex;
            if(Vertex::Cmp()(e_a,tl_a))
            {
                topLeft = c;
            }
            else
            {
                auto & br = m_edges[bottomRight];
                auto & br_a = m_vertices[br.m_node_a].m_node.m_vertex;
                if(Vertex::Cmp()(br_a,e_a))
                {
                    bottomRight = c;
                }
            }
            newEdges.erase(c);
            c = e.m_next;
        }
        newEdges.erase(edge);
        // create diagonals between left and right part of the loop
        size_t left = topLeft;
        size_t right = m_edges[topLeft].m_prev;
        while(m_edges[left].m_next != m_edges[right].m_prev)
        {
            auto & le = m_edges[left];
            auto & re = m_edges[right];
            auto [e1,e2] = AddEdgeInLoop(le.m_node_b,re.m_node_a,left,le.m_next,re.m_prev,right);
            auto & added = m_edges[e2];
            if(Vertex::Cmp()(m_vertices[m_edges[added.m_next].m_node_b],
                             m_vertices[m_edges[added.m_prev].m_node_a]))
            {
                left = e2;
                right = added.m_prev;
            }
            else
            {
                left = added.m_next;
                right = e2;
            }
        }

    }
    for(size_t i=0;i<m_edges.size();++i)
    {
        m_edges[i].m_index = i;
    }
}
