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

        void Execute();

    protected:
        std::tuple<std::multiset<SweepLine*,SweepLineCompare>,std::set<SweepNode*,SweepNodeCompare>> CollectSweepLines();

    private:
        Core::TempMemPool<SweepNode,1024> m_sweepNodePool;
        Core::TempMemPool<SweepLine,512> m_sweepLinePool;

        const std::vector<Polygon2D> m_input;
        std::vector<Polygon2D> m_output;
    };
}
}