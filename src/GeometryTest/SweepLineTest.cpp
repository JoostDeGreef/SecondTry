#include "gtest/gtest.h"
#include "Geometry.h"

#include "internal/Operations/Vector2dCompare.h"
#include "internal/Operations/SweepLine.h"
#include "internal/Operations/SweepNode.h"
#include "internal/Operations/Sweeper.h"

using namespace std;
using namespace testing;
using namespace Geometry;
using namespace Operations;

class SweepLineTest : public Test
{
protected:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(SweepLineTest, GetBottomLeftVertexPtr)
{
    SweepNode p0({0,0});
    SweepNode p1({1,1});
    SweepLine l0(&p0,&p1,{},{});
    EXPECT_EQ(&p0,l0.GetVertexPtr(0));
    SweepLine l1(&p1,&p0,{},{});
    EXPECT_EQ(&p0,l1.GetVertexPtr(0));
}

TEST_F(SweepLineTest, DetermineIntersection_BeginBegin)
{
    SweepNode p00({0,0});
    SweepNode p01({1,0});
    SweepNode p10({0,0});
    SweepNode p11({1,1});
    SweepLine l0(&p00,&p01,{},{});
    SweepLine l1(&p10,&p11,{},{});
    auto intersection = l0.DetermineIntersection(l1);
    EXPECT_FLOAT_EQ(0.0,intersection.GetS0());
    EXPECT_FLOAT_EQ(0.0,intersection.GetS1());
    EXPECT_EQ(SweepLine::Intersection::Type::Cross,intersection.GetType());
}

TEST_F(SweepLineTest, DetermineIntersection_EndBegin)
{
    SweepNode p00({0,0});
    SweepNode p01({1,0});
    SweepNode p10({1,0});
    SweepNode p11({1,1});
    SweepLine l0(&p00,&p01,{},{});
    SweepLine l1(&p10,&p11,{},{});
    auto intersection = l0.DetermineIntersection(l1);
    EXPECT_FLOAT_EQ(1.0,intersection.GetS0());
    EXPECT_FLOAT_EQ(0.0,intersection.GetS1());
    EXPECT_EQ(SweepLine::Intersection::Type::Cross,intersection.GetType());
}

TEST_F(SweepLineTest, DetermineIntersection_BeginEnd)
{
    SweepNode p00({0,0});
    SweepNode p01({1,0});
    SweepNode p10({1,1});
    SweepNode p11({0,0});
    SweepLine l0(&p00,&p01,{},{});
    SweepLine l1(&p10,&p11,{},{});
    auto intersection = l0.DetermineIntersection(l1);
    EXPECT_FLOAT_EQ(0.0,intersection.GetS0());
    EXPECT_FLOAT_EQ(0.0,intersection.GetS1());
    EXPECT_EQ(SweepLine::Intersection::Type::Cross,intersection.GetType());
}

TEST_F(SweepLineTest, DetermineIntersection_EndEnd)
{
    SweepNode p00({0,0});
    SweepNode p01({1,3});
    SweepNode p10({1,1});
    SweepNode p11({1,3});
    SweepLine l0(&p00,&p01,{},{});
    SweepLine l1(&p10,&p11,{},{});
    auto intersection = l0.DetermineIntersection(l1);
    EXPECT_FLOAT_EQ(1.0,intersection.GetS0());
    EXPECT_FLOAT_EQ(1.0,intersection.GetS1());
    EXPECT_EQ(SweepLine::Intersection::Type::Cross,intersection.GetType());
}
