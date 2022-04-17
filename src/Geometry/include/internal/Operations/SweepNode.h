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

    // move these to a separate file
    inline
    bool operator < (const Core::Vector2d & a,const Core::Vector2d & b)
    {
            const double eps = 1e-6;
            return a[1] < b[1] + eps ? true
                 : b[1] < a[1] + eps ? false
                 : a[0] < b[0] + eps ? true
                 :                     false;
    }
    inline
    bool operator > (const Core::Vector2d & a,const Core::Vector2d & b)
    {
            const double eps = 1e-6;
            return a[1] + eps > b[1] ? true
                 : b[1] + eps > a[1] ? false
                 : a[0] + eps > b[0] ? true
                 :                     false;
    }
    inline
    bool operator == (const Core::Vector2d & a,const Core::Vector2d & b)
    {
            const double eps = 1e-6;
            return a[1] > b[1] + eps ? false
                 : a[1] + eps < b[1] ? false
                 : a[0] > b[0] + eps ? false
                 : a[0] + eps < b[0] ? false
                 :                     true;
    }

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
}
}
