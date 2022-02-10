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

