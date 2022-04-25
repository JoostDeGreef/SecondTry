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
                  const std::set<size_t> & polygon0,const std::set<size_t> & polygon1)
            : m_vertices{{p0,p1}}
            , m_polygons{{polygon0,polygon1}}
        {
            SetVertexOrder();
        }

        const SweepNode & GetVertex(const size_t & index) const { return *GetVertexPtr(index); }
        const SweepNode * GetVertexPtr(const size_t & index) const { return m_vertices[index]; }
        void SetVertexPtr(const size_t & index, SweepNode * vertexPtr) { m_vertices[index] = vertexPtr; }
        void SetVertexOrder();

        class Intersection
        {
        public:
            enum class Type
            {
                None,
                Cross,
                Overlapping
            };

            const Type & GetType() const { return m_type; }
            const double & GetS0() const { return m_S[0]; }
            const double & GetS1() const { return m_S[1]; }

            Intersection(const Type t, const double s0, const double s1)
                : m_type(t)
                , m_S({s0,s1})
            {}
        private:
            const Type m_type;
            const std::array<double,2> m_S;
        };

        Intersection DetermineIntersection(const SweepLine & other) const;
    private:
        std::array<SweepNode *,2> m_vertices; // bottom, top
        std::array<std::set<size_t>,2> m_polygons;
    };

    struct SweepLineCompare
    {
        bool operator () (const SweepLine & a, const SweepLine & b) const;
        bool operator () (const SweepLine * a, const SweepLine * b) const
        {
            return operator()(*a,*b);
        }
    };
}
}
