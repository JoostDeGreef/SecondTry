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
    double rmax = 2;
    double rmin = 2 * 0.95;
    auto cylinder = Shape::Construct::Cylinder(5,rmax,rmin);
    double vmax = 5 * Core::Constants::Pi * pow(rmax,2);
    double vmin = 5 * Core::Constants::Pi * pow(rmin,2);
    double volume = cylinder.CalculateVolume();
    EXPECT_GT(vmax,volume);
    EXPECT_LT(vmin,volume);
    double smax = 5*2*Core::Constants::Pi*rmax + 2*Core::Constants::Pi*pow(rmax,2);
    double smin = 5*2*Core::Constants::Pi*rmin + 2*Core::Constants::Pi*pow(rmin,2);
    double surface = cylinder.CalculateSurface();
    EXPECT_GT(smax,surface);
    EXPECT_LT(smin,surface);
}
