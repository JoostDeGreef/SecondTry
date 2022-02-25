#include "gtest/gtest.h"
#include "Geometry.h"

using namespace std;
using namespace testing;
using namespace Geometry;

class PolygonTest : public Test
{
protected:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

    auto GetSamplePolygon()
    {
        Polygon2D p;
        std::vector<Core::Vector2d> vertices = 
        {
            {15,10},
            {13, 8},
            {12,14},
            { 9,13},
            { 7,15},
            { 3,12},
            { 6,11},
            { 5, 7},
            { 2, 9},
            { 1, 5},
            { 4, 2},
            { 8, 3},
            {11, 1},
            {10, 6},
            {14, 4}
        };
        for(auto v:vertices)
        {
            p.AddNode(v);
        }
        return p;
    }

};

namespace Core
{
    // TODO: move this to a separate header?
    bool operator == (const Core::TVector<double, 2> & a,const Core::TVector<double, 2> & b)
    {
        return a[0] == b[0] && a[1] == b[1];
    }

    void PrintTo(const Core::TVector<double, 2>& v, std::ostream* os) 
    {
        *os << "[" << v[0] << "," << v[1] << "]"; 
    }
}

class TriangulatedPolygon2DTest: public TriangulatedPolygon2D
{
public:
    TriangulatedPolygon2DTest()
        : TriangulatedPolygon2D(Polygon2D())
    {}

    using TriangulatedPolygon2D::TriangulatedPolygon2D;
    using TriangulatedPolygon2D::angle;
    using TriangulatedPolygon2D::AddEdge;
    using TriangulatedPolygon2D::CopyInputPolygonToVertices;
    using TriangulatedPolygon2D::InitializeVertexType;
    using TriangulatedPolygon2D::SplitInMonotonePieces;
    using TriangulatedPolygon2D::TriangulateInputPolygon;

    using TriangulatedPolygon2D::m_vertices;
    using TriangulatedPolygon2D::m_edges;
};

TEST_F(PolygonTest, IsCounterClockwise)
{
    Polygon2D p;
    p.AddNode({0,0});
    p.AddNode({1,0});
    p.AddNode({1,1});
    p.AddNode({0,1});
    EXPECT_TRUE(p.IsCounterClockwise());
}

TEST_F(PolygonTest, IsClockwise)
{
    Polygon2D p;
    p.AddNode({0,0});
    p.AddNode({0,1});
    p.AddNode({1,1});
    p.AddNode({1,0});
    EXPECT_FALSE(p.IsCounterClockwise());
}

TEST_F(PolygonTest, Angle)
{
    Core::Vector2d v0(1,0);
    Core::Vector2d v1(1,1);
    double a0 = TriangulatedPolygon2DTest::angle(v0,v1);
    double a1 = TriangulatedPolygon2DTest::angle(v1,v0);
    EXPECT_LT(0,a0);
    EXPECT_GT(0,a1);
    EXPECT_FLOAT_EQ(a0,-a1);
}

TEST_F(PolygonTest, Cmp)
{
    Core::Vector2d a(1,0);
    Core::Vector2d b(1,1);
    Core::Vector2d c(0,1);
    EXPECT_TRUE(TriangulatedPolygon2DTest::Vertex::Cmp()(b,a));
    EXPECT_FALSE(TriangulatedPolygon2DTest::Vertex::Cmp()(a,b));
    EXPECT_TRUE(TriangulatedPolygon2DTest::Vertex::Cmp()(c,b));
}

TEST_F(PolygonTest, CopyInputPolygonToVertices)
{
    Polygon2D p0,p1;
    p0.AddNode({0,0});
    p0.AddNode({1,0});
    p0.AddNode({1,1});
    p0.AddNode({0,1});
    p1.AddNode({0,1});
    p1.AddNode({1,1});
    p1.AddNode({1,0});
    p1.AddNode({0,0});
    TriangulatedPolygon2DTest tp0;
    TriangulatedPolygon2DTest tp1;
    tp0.CopyInputPolygonToVertices(p0);
    tp1.CopyInputPolygonToVertices(p1);
    EXPECT_EQ(4,tp0.m_vertices.size());
    EXPECT_EQ(4,tp1.m_vertices.size());
    EXPECT_EQ(tp0.m_vertices[0].m_node.m_vertex,tp1.m_vertices[0].m_node.m_vertex);
    EXPECT_EQ(tp0.m_vertices[1].m_node.m_vertex,tp1.m_vertices[1].m_node.m_vertex);
    EXPECT_EQ(tp0.m_vertices[2].m_node.m_vertex,tp1.m_vertices[2].m_node.m_vertex);
    EXPECT_EQ(tp0.m_vertices[3].m_node.m_vertex,tp1.m_vertices[3].m_node.m_vertex);
}

TEST_F(PolygonTest, AddEdge)
{
    Polygon2D p;
    p.AddNode({0,0});
    p.AddNode({1,0});
    p.AddNode({1,1});
    p.AddNode({0,1});
    TriangulatedPolygon2DTest tp;
    tp.CopyInputPolygonToVertices(p);
    tp.InitializeVertexType();
    EXPECT_EQ(4,tp.m_edges.size());
    tp.AddEdge(1,3);
    EXPECT_EQ(6,tp.m_edges.size());
}

TEST_F(PolygonTest, InitializeVertexType)
{
    Polygon2D p = GetSamplePolygon();
    TriangulatedPolygon2DTest tp;
    tp.CopyInputPolygonToVertices(p);
    tp.InitializeVertexType();
    EXPECT_EQ(15,tp.m_vertices.size());
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Start,tp.m_vertices[0].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Merge,tp.m_vertices[1].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Start,tp.m_vertices[2].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Merge,tp.m_vertices[3].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Start,tp.m_vertices[4].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Regular,tp.m_vertices[5].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Regular,tp.m_vertices[6].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Merge,tp.m_vertices[7].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Start,tp.m_vertices[8].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Regular,tp.m_vertices[9].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::End,tp.m_vertices[10].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Split,tp.m_vertices[11].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::End,tp.m_vertices[12].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::Split,tp.m_vertices[13].m_type);
    EXPECT_EQ(TriangulatedPolygon2D::Vertex::Type::End,tp.m_vertices[14].m_type);
}

TEST_F(PolygonTest, SplitInMonotonePieces)
{
    Polygon2D p = GetSamplePolygon();
    TriangulatedPolygon2DTest tp;
    tp.CopyInputPolygonToVertices(p);
    tp.InitializeVertexType();
    EXPECT_EQ(15,tp.m_vertices.size());
    EXPECT_EQ(15,tp.m_edges.size());
    tp.SplitInMonotonePieces();
    EXPECT_EQ(15,tp.m_vertices.size());
    EXPECT_EQ(23,tp.m_edges.size());
    EXPECT_EQ(3,tp.m_edges[tp.m_edges[tp.m_edges[3].m_next].m_next].m_next);
    EXPECT_EQ(3,tp.m_edges[tp.m_edges[tp.m_edges[3].m_prev].m_prev].m_prev);
    EXPECT_EQ(10,tp.m_edges[tp.m_edges[tp.m_edges[10].m_next].m_next].m_next);
    EXPECT_EQ(10,tp.m_edges[tp.m_edges[tp.m_edges[10].m_prev].m_prev].m_prev);
}

TEST_F(PolygonTest, Triangulate)
{
    Polygon2D p = GetSamplePolygon();
    TriangulatedPolygon2DTest tp;
    tp.CopyInputPolygonToVertices(p);
    EXPECT_EQ(15,tp.m_vertices.size());
    tp.TriangulateInputPolygon();
    EXPECT_EQ(15,tp.m_vertices.size());
    EXPECT_EQ(39,tp.m_edges.size());
    for(size_t i=0;i<tp.m_edges.size();++i)
    {
        EXPECT_EQ(i,tp.m_edges[tp.m_edges[tp.m_edges[i].m_next].m_next].m_next);
        EXPECT_EQ(i,tp.m_edges[tp.m_edges[tp.m_edges[i].m_prev].m_prev].m_prev);
    }
}

TEST_F(PolygonTest, TriangulateMonotone)
{
    Polygon2D p;
    p.AddNode({0,0});
    p.AddNode({1,0});
    p.AddNode({1,1});
    p.AddNode({0,1});
    TriangulatedPolygon2DTest tp(p);
    EXPECT_EQ(6,tp.m_edges.size());
}

TEST_F(PolygonTest, TriangulateSplit)
{
    Polygon2D p;
    p.AddNode({0.0,0.0});
    p.AddNode({1.0,0.0});
    p.AddNode({1.0,1.0});
    p.AddNode({0.5,0.5});
    p.AddNode({0.0,1.0});
    TriangulatedPolygon2DTest tp(p);
    EXPECT_EQ(9,tp.m_edges.size());
}
