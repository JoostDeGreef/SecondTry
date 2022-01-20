#include "gtest/gtest.h"
#include "Geometry.h"

using namespace std;
using namespace testing;
using namespace Geometry;

class ShapeTest : public Test
{
protected:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(ShapeTest, Cube)
{
    auto cube = Shape::Construct::Cube(1);
    // todo: calculate volume
}

TEST_F(ShapeTest, Box)
{
    auto box = Shape::Construct::Box({1,1.1,1.2});
    // todo: calculate volume
}
