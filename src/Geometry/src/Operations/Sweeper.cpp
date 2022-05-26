#include "Geometry.h"

#include "internal/Operations/Vector2dCompare.h"
#include "internal/Operations/SweepLine.h"
#include "internal/Operations/SweepNode.h"
#include "internal/Operations/SweepEvent.h"
#include "internal/Operations/Sweeper.h"

using namespace Core;
using namespace Geometry;
using namespace Operations;

void Sweeper::Execute()
{
    // sorted set of sweeplines and sweepnodes
    auto [sweepLines,sweepNodes] = CollectSweepLines();

//for debugging
for(auto & n:sweepNodes)
{
    std::cout << n->GetVertex() << " ( ";
    for(auto & l:n->GetLines())
    {
        std::cout << "(" << l->GetVertex(0).GetVertex() << "->" << l->GetVertex(1).GetVertex() << ") ";
    }
    std::cout << ")" << std::endl;
}

    // Add nodes as Events
    std::set<SweepEvent,SweepEventCompare> events;
    std::multiset<SweepLine *,EventLineCompare> eventLine;
    for(auto & node:sweepNodes)
    {
        events.emplace(node);
    }
    // Handle all events
    auto SplitLineAtNode = [&](SweepLine * line,SweepNode * node)
    {
        SweepNode * oldNode = line->GetVertexPtr(1);
        SweepLine * newLine = m_sweepLinePool.Alloc(node,oldNode,line->GetPolygons(0),line->GetPolygons(1));
        line->SetVertexPtr(1,node);
        oldNode->RemoveLine(line);
        oldNode->AddLine(newLine);
        node->AddLine(newLine);
        node->AddLine(line);
        sweepLines.emplace(newLine);
    };
    auto HandleCrossIntersection = [&](SweepLine * a,SweepLine * b,SweepLine::Intersection & intersection)
    {
        // todo:
        // - create/reuse SweepNode
        // - update events if Sweepnode created
        auto s0 = intersection.GetS0();
        auto s1 = intersection.GetS1();
        auto va0 = a->GetVertexPtr(0);
        auto va1 = a->GetVertexPtr(1);
        auto vb0 = b->GetVertexPtr(0);
        auto vb1 = b->GetVertexPtr(1);
        auto v = (va0->GetVertex()*(1-s0) + va1->GetVertex()*(s0) + vb0->GetVertex()*(1-s1) + vb1->GetVertex()*(s1))*0.5;

std::cout << "HandleCrossIntersection: ("
          << va0->GetVertex() << ") -> (" << va1->GetVertex() << ") intersects with (" 
          << vb0->GetVertex() << ") -> (" << vb1->GetVertex() << ") at (" 
          << v << ")" << std::endl;

        auto sweepNode = m_sweepNodePool.Alloc(v);
        auto nodeIter = sweepNodes.emplace(sweepNode);
        SweepNode * node = *(nodeIter.first);
        if(va0 == node || va1 == node)
        {
            if(vb0 == node || vb1 == node)
            {
                // already connected
            }
            else
            {
                // split b at node
                SplitLineAtNode(b,node);
                events.emplace(node);
            }
        }
        else if(vb0 == node || vb1 == node)
        {
            // split a at node
            SplitLineAtNode(a,node);
            events.emplace(node);
        }
        else
        {
            // split a and b at node
            SplitLineAtNode(a,node);
            SplitLineAtNode(b,node);
            events.emplace(node);
        }
    };
    auto HandleOverlappingIntersection = [&](SweepLine * a,SweepLine * b,SweepLine::Intersection & intersection)
    {
        // todo
    };
    auto HandlePossibleIntersection = [&](SweepLine * a,SweepLine * b)
    {
        if(a != b)
        {
            auto intersection = a->DetermineIntersection(*b);
            switch(intersection.GetType())
            {
                case SweepLine::Intersection::Type::None:
                    break;
                case SweepLine::Intersection::Type::Cross:
                    HandleCrossIntersection(a,b,intersection);
                    break;
                case SweepLine::Intersection::Type::Overlapping:
                    HandleOverlappingIntersection(a,b,intersection);
                    break;
            }
        }
    };
    auto GetNeighborhoodRange = [&](std::multiset<SweepLine *, EventLineCompare>::iterator & where)
    {
        // find the first line which needs to be checked for intersections
        auto start = where;
        while(start != eventLine.begin() && EventLineCompare::Equal(*start,*where))
        {
            --start;
        }
        if(start != eventLine.begin())
        {
            auto temp = --start;
            while(temp != eventLine.begin() && EventLineCompare::Equal(*start,*temp))
            {
                start = temp;
                --temp;
            }
        }
        // find the last line + 1 which needs to be checked for intersections
        auto end = where;
        while(end != eventLine.end() && EventLineCompare::Equal(*end,*where))
        {
            ++end;
        }
        if(end != eventLine.end())
        {
            auto temp = ++end;
            while(temp != eventLine.end() && EventLineCompare::Equal(*end,*temp))
            {
                end = temp;
                ++temp;
            }
        }
        return std::make_tuple(start,end);
    };
    auto AddLine = [&](SweepLine * line)
    {
std::cout << "AddLine: " << line->GetVertex(0).GetVertex() << " -> " << line->GetVertex(1).GetVertex() << std::endl;

        auto where = eventLine.emplace(line);
        auto [start, end] = GetNeighborhoodRange(where);
        // check all neighbors for intersection with line/where
        for(auto iter = start; iter != end; ++iter)
        {
            if(iter != where)
            {
                HandlePossibleIntersection(*iter,*where);
            }
        }
    };
    auto RemoveLine = [&](SweepLine * line)
    {
std::cout << "removeLine: " << line->GetVertex(0).GetVertex() << " -> " << line->GetVertex(1).GetVertex() << std::endl;

        auto where = eventLine.find(line);
        auto [start, end] = GetNeighborhoodRange(where);
        // check all neighbors for intersection with line/where
        for(auto i = start; i != end; ++i)
        {
            for(auto j = i; j != end; ++j)
            {
                if(i != j)
                {
                    HandlePossibleIntersection(*i,*j);
                }
            }
        }
        eventLine.erase(where);
    };
    //for debugging
    auto DisplayEventLine = [&]()
    {
        std::cout << "------------------------------------" << std::endl << " -- ";
        for(auto & l:eventLine)
        {
            std::cout << l->GetVertex(0).GetVertex() << " -- ";
        }
        std::cout << std::endl << " -- ";
        for(auto & l:eventLine)
        {
            std::cout << l->GetVertex(1).GetVertex() << " -- ";
        }
        std::cout << std::endl;
    };
    auto DisplayEvents = [&]()
    {
        for(auto & e:events)
        {
            std::cout << e.GetNode()->GetVertex() << " ";
        }
        std::cout << std::endl;
    };
    while( !events.empty() )
    {
        DisplayEvents();
        DisplayEventLine();
        // get the first unhandled event
        auto iterFirst = events.begin();
        auto firstEvent = *iterFirst;
        events.erase(iterFirst);
        // add the start sweepline(s) to the eventline
        auto node = firstEvent.GetNode();
        for(auto & startLine:node->GetLines())
        {
            if(startLine->GetVertexPtr(0) == node)
            {
                AddLine(startLine);
            }
        }
        //DisplayEvents();
        //DisplayEventLine();
        // remove the end sweepline(s) from the eventline
        for(auto & endLine:node->GetLines())
        {
            if(endLine->GetVertexPtr(1) == node)
            {
                RemoveLine(endLine);
            }
        }
    }
}

std::tuple<std::multiset<SweepLine*,SweepLineCompare>,std::set<SweepNode*,SweepNodeCompare>> Sweeper::CollectSweepLines()
{
    // todo: store original node index and line index in sweep equivalents
    std::set<SweepNode*,SweepNodeCompare> sweepNodes;
    std::multiset<SweepLine*,SweepLineCompare> sweeplines;
    // create sweeplines per polygon and add them to the set
    for(size_t i=0;i<m_input.size();i++)
    {
        std::set<size_t> leftPolygons{{i}};
        std::set<size_t> rightPolygons;
        if(!m_input[i].IsCounterClockwise())
        {
            std::swap(leftPolygons,rightPolygons);
        }
        auto & nodes = m_input[i].GetNodes();
        std::vector<SweepNode*> vertices;
        vertices.resize(nodes.size());
        for(size_t j=0;j<nodes.size();j++)
        {
            // use matching/existing node if present
            auto sweepNode = m_sweepNodePool.Alloc(nodes[j].m_vertex);
            vertices[j] = *(sweepNodes.emplace(sweepNode).first);
        }
        // remove duplicated nodes (0-length lines). use the sweepnode ptr, real comparison is already done
        auto last = std::unique(vertices.begin(), vertices.end());
        vertices.erase(last, vertices.end());
        // also check first vs last node
        if( vertices.size()>1 && vertices.back() == vertices.front() )
        {
            vertices.pop_back();
        }
        // create lines between vertices
        std::vector<SweepLine*> tempsweeplines;
        tempsweeplines.resize(vertices.size());
        size_t k = vertices.size()-1;
        for(size_t j=0;j<vertices.size();j++)
        {
            tempsweeplines[j] = m_sweepLinePool.Alloc(vertices[k],vertices[j],leftPolygons,rightPolygons);
            vertices[k]->AddLine(tempsweeplines[j]);
            vertices[j]->AddLine(tempsweeplines[j]);
            k = j;
        }
        for(auto sweepline:tempsweeplines)
        {
            sweeplines.emplace(sweepline);
        }
    }
    return std::make_tuple(sweeplines,sweepNodes);
}

std::vector<Polygon2D> Sweeper::GetUnion()
{
    return m_output;
}

