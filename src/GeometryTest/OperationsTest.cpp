#include "gtest/gtest.h"
#include "Geometry.h"

using namespace std;
using namespace testing;
using namespace Geometry;

class OperationsTest : public Test
{
protected:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(OperationsTest, Union2D_no_overlap)
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
    auto res = Operations::Union(p0,p1);
    ASSERT_EQ(2,res.size());
    EXPECT_EQ(p0.Surface(),res[0].Surface());
    EXPECT_EQ(p1.Surface(),res[1].Surface());
}

TEST_F(OperationsTest, Union2D_overlap)
{
    Polygon2D p0,p1;
    p0.AddNode({0,0});
    p0.AddNode({1,0});
    p0.AddNode({1,1});
    p0.AddNode({0,1});
    p1.AddNode({0.5,0.5});
    p1.AddNode({1.5,0.5});
    p1.AddNode({1.5,1.5});
    p1.AddNode({0.5,1.5});
    auto res = Operations::Union(p0,p1);
    ASSERT_EQ(1,res.size());
    EXPECT_EQ(p0.Surface()*0.75+p1.Surface(),res[0].Surface());
}

TEST_F(OperationsTest, Union2D_touching)
{
    Polygon2D p0,p1;
    p0.AddNode({0,0});
    p0.AddNode({1,0});
    p0.AddNode({1,1});
    p0.AddNode({0,1});
    p1.AddNode({1,0});
    p1.AddNode({2,0});
    p1.AddNode({2,1});
    p1.AddNode({1,1});
    auto res = Operations::Union(p0,p1);
    ASSERT_EQ(1,res.size());
    EXPECT_EQ(p1.Surface()+p0.Surface(),res[0].Surface());
}
