#include "gtest/gtest.h"
#include "Core.h"

using namespace std;
using namespace testing;
using namespace Core;

class TempMemPoolTest : public Test
{
protected:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(TempMemPoolTest, BasicFunctionality)
{
    TempMemPool<int,1> store;

    auto a = store.Alloc();
    EXPECT_EQ(0,*a);

    auto b = store.Alloc(42);
    EXPECT_EQ(42,*b);
}

