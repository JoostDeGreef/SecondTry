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
};

class TriangulatedPolygon2DTest: public TriangulatedPolygon2D
{
public:
    using TriangulatedPolygon2D::TriangulatedPolygon2D;
    using TriangulatedPolygon2D::angle;

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
