#include "gtest/gtest.h"

#include "Core.h"
#include "Geometry.h"

#include "internal/Operations/Vector2dCompare.h"

using namespace std;
using namespace testing;
using namespace Geometry;
using namespace Operations;

class Vector2dCompareTest : public Test
{
protected:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(Vector2dCompareTest, SmallerThan)
{
    Core::Vector2d a(1,0);
    Core::Vector2d b(0,1);
    Core::Vector2d c(0,0);
    EXPECT_TRUE(a<b);
    EXPECT_FALSE(b<a);
    EXPECT_TRUE(c<a);
    EXPECT_TRUE(c<b);
}

