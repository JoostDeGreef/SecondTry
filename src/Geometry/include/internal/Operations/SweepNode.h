#pragma once

namespace Geometry
{
namespace Operations
{
    class SweepLine;

    class SweepNode
    {
    public:
        SweepNode(const Core::Vector2d & p)
            : m_vertex{p}
            , m_sweeplines()
        {}

        const Core::Vector2d & GetVertex() const { return m_vertex; }
        void SetVertex(const Core::Vector2d & vertex) { m_vertex = vertex; }

        void AddLine(SweepLine * sweepline) { m_sweeplines.emplace_back(sweepline); }
    private:
        Core::Vector2d m_vertex;
        std::vector<SweepLine*> m_sweeplines;
    };

    struct SweepNodeCompare
    {
        bool operator () (const SweepNode & a, const SweepNode & b) const
        {
            return false;
        }
        bool operator () (const SweepNode * a, const SweepNode * b) const
        {
            return operator()(*a,*b);
        }
    };
}
}
