#include "gtest/gtest.h"
#include "Geometry.h"

#include "internal/Operations/Vector2dCompare.h"
#include "internal/Operations/SweepLine.h"
#include "internal/Operations/SweepNode.h"
#include "internal/Operations/Sweeper.h"

using namespace std;
using namespace testing;
using namespace Geometry;

using Sweeper = Operations::Sweeper;
class SweeperWrapper : public Sweeper
{
public:
    using Sweeper::Sweeper;
    using Sweeper::CollectSweepLines;
    using Sweeper::Execute;
};

class SweeperTest : public Test
{
protected:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(SweeperTest, CollectSweepLines)
{
    Polygon2D p0;
    p0.AddNode({0,0});
    p0.AddNode({1,0});
    p0.AddNode({1,1});
    p0.AddNode({0,1});
    EXPECT_TRUE(p0.IsCounterClockwise());
    SweeperWrapper s({p0});
    auto [sweeplines,sweepnodes] = s.CollectSweepLines();
    EXPECT_EQ(4,sweeplines.size());
    EXPECT_EQ(4,sweepnodes.size());
}

TEST_F(SweeperTest, CollectSweepLinesDisjoint)
{
    Polygon2D p0,p1;
    p0.AddNode({0,0});
    p0.AddNode({1,0});
    p0.AddNode({1,1});
    p0.AddNode({0,1});
    p1.AddNode({2,0});
    p1.AddNode({3,0});
    p1.AddNode({3,1});
    p1.AddNode({2,1});
    EXPECT_TRUE(p0.IsCounterClockwise());
    EXPECT_TRUE(p1.IsCounterClockwise());
    SweeperWrapper s({p0,p1});
    auto [sweeplines,sweepnodes] = s.CollectSweepLines();
    EXPECT_EQ(8,sweeplines.size());
    EXPECT_EQ(8,sweepnodes.size());
}

TEST_F(SweeperTest, CollectSweepLinesCross)
{
    Polygon2D p0,p1;
    p0.AddNode({0,0});
    p0.AddNode({2,0});
    p0.AddNode({2,2});
    p0.AddNode({0,2});
    p1.AddNode({1,1});
    p1.AddNode({3,1});
    p1.AddNode({3,3});
    p1.AddNode({1,3});
    EXPECT_TRUE(p0.IsCounterClockwise());
    EXPECT_TRUE(p1.IsCounterClockwise());
    SweeperWrapper s({p0,p1});
    auto [sweeplines,sweepnodes] = s.CollectSweepLines();
    EXPECT_EQ(8,sweeplines.size());
    EXPECT_EQ(8,sweepnodes.size());
}

TEST_F(SweeperTest, CollectSweepLinesWithShortLine)
{
    Polygon2D p0;
    p0.AddNode({0,0});
    p0.AddNode({1,0});
    p0.AddNode({1,1});
    p0.AddNode({1,1+1e-7});
    EXPECT_TRUE(p0.IsCounterClockwise());
    SweeperWrapper s({p0});
    auto [sweeplines,sweepnodes] = s.CollectSweepLines();
    EXPECT_EQ(3,sweeplines.size());
    EXPECT_EQ(3,sweepnodes.size());
}

TEST_F(SweeperTest, ExecuteDisjoint)
{
    Polygon2D p0,p1;
    p0.AddNode({0,0});
    p0.AddNode({1,0});
    p0.AddNode({1,1});
    p0.AddNode({0,1});
    p1.AddNode({2,0});
    p1.AddNode({3,0});
    p1.AddNode({3,1});
    p1.AddNode({2,1});
    EXPECT_TRUE(p0.IsCounterClockwise());
    EXPECT_TRUE(p1.IsCounterClockwise());
    SweeperWrapper s({p0,p1});
    s.Execute();
    auto res = s.GetUnion();
    EXPECT_EQ(2,res.size());
}

TEST_F(SweeperTest, ExecuteCross)
{
    Polygon2D p0,p1;
    p0.AddNode({0,0});
    p0.AddNode({2,0});
    p0.AddNode({2,2});
    p0.AddNode({0,2});
    p1.AddNode({1,1});
    p1.AddNode({3,1});
    p1.AddNode({3,3});
    p1.AddNode({1,3});
    EXPECT_TRUE(p0.IsCounterClockwise());
    EXPECT_TRUE(p1.IsCounterClockwise());
    SweeperWrapper s({p0,p1});
    s.Execute();
    auto res = s.GetUnion();
    EXPECT_EQ(1,res.size());
}

