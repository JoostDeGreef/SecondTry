
#include "gtest/gtest.h"

using namespace testing;

class OutputTest : public Test
{
protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(OutputTest, FooBar)
{
    SUCCEED() << "Some Text";
    EXPECT_TRUE(1 == 1) << "For some reason this doesn't work?";
}


