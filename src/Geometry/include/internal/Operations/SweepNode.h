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

        void AddLine(SweepLine * sweepline) { m_sweeplines.emplace(sweepline); }
        void RemoveLine(SweepLine * sweepline) { m_sweeplines.erase(sweepline); }
        const std::set<SweepLine*> & GetLines() const { return m_sweeplines; }
    private:
        Core::Vector2d m_vertex;
        std::set<SweepLine*> m_sweeplines;
    };

    // Nodes are ordered bottom -> top, left -> right
    struct SweepNodeCompare
    {
        bool operator () (const Core::Vector2d & a,const Core::Vector2d & b) const
        {
            return a < b;
        }
        bool operator () (const SweepNode & a, const SweepNode & b) const
        {
            return operator()(a.GetVertex(),b.GetVertex());
        }
        bool operator () (const SweepNode * a, const SweepNode * b) const
        {
            return operator()(*a,*b);
        }
    };

    // Nodes are ordered left -> right only
    struct EventNodeCompare
    {
        bool operator () (const Core::Vector2d & a,const Core::Vector2d & b) const
        {
            return Less(a[0],b[0]);
        }
        bool operator () (const SweepNode & a, const SweepNode & b) const
        {
            return operator()(a.GetVertex(),b.GetVertex());
        }
        bool operator () (const SweepNode * a, const SweepNode * b) const
        {
            return operator()(*a,*b);
        }
    };
}
}
