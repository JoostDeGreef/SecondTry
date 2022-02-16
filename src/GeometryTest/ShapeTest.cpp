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
    double volume = cube.CalculateVolume();
    EXPECT_FLOAT_EQ(1.0,volume);
    double surface = cube.CalculateSurface();
    EXPECT_FLOAT_EQ(6.0,surface);
}

TEST_F(ShapeTest, Box)
{
    auto box = Shape::Construct::Box({1,1.2,1.5});
    double volume = box.CalculateVolume();
    EXPECT_FLOAT_EQ(1.8,volume);
    double surface = box.CalculateSurface();
    EXPECT_FLOAT_EQ(9.0,surface);
}

TEST_F(ShapeTest, Cylinder)
{
    auto cylinder = Shape::Construct::Cylinder(5,2);
    double volume = cylinder.CalculateVolume();
    EXPECT_FLOAT_EQ(1.8,volume);
    double surface = cylinder.CalculateSurface();
    EXPECT_FLOAT_EQ(9.0,surface);
}
