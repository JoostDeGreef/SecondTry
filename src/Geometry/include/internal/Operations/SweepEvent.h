#pragma once

namespace Geometry
{
namespace Operations
{
    class SweepEvent
    {
    public:
        SweepEvent(SweepNode * node)
            : m_node(node)
        {}

        SweepNode * GetNode() const { return m_node; }
    protected:
    private:
        SweepNode * m_node;
    };

    struct SweepEventCompare
    {
        bool operator () (const SweepEvent & a, const SweepEvent & b) const
        {
            return SweepNodeCompare()(a.GetNode(),b.GetNode());
        }
    };
}
}
