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
    // Add nodes as Events
    std::set<SweepEvent,SweepEventCompare> events;
    std::multiset<const SweepLine *,EventLineCompare> eventLine;
    for(auto & node:sweepNodes)
    {
        events.emplace(node);
    }
    // Handle all events
    auto HandlePossibleIntersection = [&](const SweepLine * a,const SweepLine * b)
    {

    };
    auto AddLine = [&](const SweepLine * line)
    {
        // todo
        auto where = eventLine.emplace(line);
        // find the first line which needs to be checked for intersections
        auto start = where;
        while(start != eventLine.begin() && EventLineCompare::Equal(*start,line))
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
        while(end != eventLine.end() && EventLineCompare::Equal(*end,line))
        {
            ++end;
        }
        if(end != eventLine.end())
        {
            auto temp = ++end;
            while(temp != eventLine.begin() && EventLineCompare::Equal(*end,*temp))
            {
                end = temp;
                --temp;
            }
        }
        // check all neighbors for intersection with line/where
        for(auto iter = start; iter != end; ++iter)
        {
            if(iter != where)
            {
                HandlePossibleIntersection(*iter,*where);
            }
        }


    };
    auto RemoveLine = [&](const SweepLine * line)
    {
        // todo
    };
    while( !events.empty() )
    {
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
        // remove the end sweepline(s) from the eventline
        for(auto & startLine:node->GetLines())
        {
            if(startLine->GetVertexPtr(0) == node)
            {
                AddLine(startLine);
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
        if( *last == vertices.front() )
        {
            // also check first vs last node
            --last;
        }
        vertices.erase(last, vertices.end());
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
