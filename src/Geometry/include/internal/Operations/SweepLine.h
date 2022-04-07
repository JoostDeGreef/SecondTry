#pragma once

namespace Geometry
{
namespace Operations
{
    class SweepNode;
    
    class SweepLine
    {
    public:
        SweepLine(SweepNode * p0, SweepNode * p1,
                  const size_t polygon,
                  const size_t leftCount, const size_t rightCount)
            : m_vertices{{p0,p1}}
            , m_polygon(polygon)
            , m_next(nullptr)
            , m_prev(nullptr)
            , m_leftCount(leftCount)
            , m_rightCount(rightCount)
        {}

        const SweepNode & GetStartVertex() const { return *GetStartVertexPtr(); }
        const SweepNode & GetEndVertex() const { return *GetEndVertexPtr(); }
        const SweepNode * GetStartVertexPtr() const { return m_vertices[0]; }
        const SweepNode * GetEndVertexPtr() const { return m_vertices[1]; }
        void SetStartVertexPtr(SweepNode * startVertexPtr) { m_vertices[0] = startVertexPtr; }
        void SetEndVertexPtr(SweepNode * endVertexPtr) { m_vertices[1] = endVertexPtr; }

        void SetNext(SweepLine * next) { m_next = next; }
        void SetPrev(SweepLine * prev) { m_prev = prev; }
        SweepLine * GetNext() const { return m_next; }
        SweepLine * GetPrev() const { return m_prev; }
    private:
        std::array<SweepNode *,2> m_vertices;
        const size_t m_polygon;
        SweepLine * m_next;
        SweepLine * m_prev;
        size_t m_leftCount;
        size_t m_rightCount;
    };

    struct SweepLineCompare
    {
        bool operator () (const SweepLine & a, const SweepLine & b) const
        {
            return false;
        }
        bool operator () (const SweepLine * a, const SweepLine * b) const
        {
            return operator()(*a,*b);
        }
    };
}
}
