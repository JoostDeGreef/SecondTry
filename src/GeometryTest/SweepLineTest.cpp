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
    SweepLine l0(&p0,&p1,0,0,0);
    EXPECT_EQ(&p0,l0.GetBottomLeftVertexPtr());
    SweepLine l1(&p1,&p0,0,0,0);
    EXPECT_EQ(&p0,l1.GetBottomLeftVertexPtr());
}
