#pragma once

namespace Geometry
{
namespace Operations
{
    class Sweeper
    {
    public:        
        Sweeper(const std::vector<Geometry::Polygon2D> & polygons)
            : m_input(polygons)
            , m_output()
        {}

        void Execute()
        {
            // sorted set of sweeplines
            auto sweeplines = CollectSweepLines();
            // Sweep through segments, add intersections


            // When nodes are found to match 'exactly', merge them.

        }

    protected:
        std::set<SweepLine*,SweepLineCompare> CollectSweepLines()
        {
            // todo: store original node index and line index in sweep equivalents
            std::set<SweepNode*,SweepNodeCompare> sweepNodes;
            std::set<SweepLine*,SweepLineCompare> sweeplines;
            // create sweeplines per polygon and add them to the set
            for(size_t i=0;i<m_input.size();i++)
            {
                size_t leftCount = m_input[i].IsCounterClockwise()?1:0;
                size_t rightCount = 1 - leftCount;
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
                    tempsweeplines[j] = m_sweepLinePool.Alloc(vertices[k],vertices[j],i,leftCount,rightCount);
                    vertices[k]->AddLine(tempsweeplines[j]);
                    vertices[j]->AddLine(tempsweeplines[j]);
                    k = j;
                }
                k = tempsweeplines.size() - 1;
                for(size_t j=0;j<vertices.size();j++)
                {
                    tempsweeplines[j]->SetPrev(tempsweeplines[k]);
                    tempsweeplines[k]->SetNext(tempsweeplines[j]);
                    k = j;
                }
                for(auto sweepline:tempsweeplines)
                {
                    sweeplines.emplace(sweepline);
                }
            }
            return sweeplines;
        }
    private:
        Core::TempMemPool<SweepNode,1024> m_sweepNodePool;
        Core::TempMemPool<SweepLine,512> m_sweepLinePool;

        const std::vector<Polygon2D> m_input;
        std::vector<Polygon2D> m_output;
    };
}
}