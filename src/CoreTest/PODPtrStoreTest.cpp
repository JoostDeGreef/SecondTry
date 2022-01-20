#include "gtest/gtest.h"
#include "Core.h"

using namespace std;
using namespace testing;
using namespace Core;

class PODPtrStoreTest : public Test
{
protected:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(PODPtrStoreTest, BasicFunctionality)
{
    PODPtrStore<int> store;

    EXPECT_EQ(0,store.UsedSlots());

    auto a = store.Alloc();
    EXPECT_EQ(1,store.UsedSlots());

    auto b = store.Attach(a);
    EXPECT_EQ(1,store.UsedSlots());

    EXPECT_EQ(a,b);

    store.Release(a);
    EXPECT_EQ(1,store.UsedSlots());

    EXPECT_EQ(nullptr,a);
    EXPECT_NE(nullptr,b);

    store.Release(b);
    EXPECT_EQ(0,store.UsedSlots());
}

TEST_F(PODPtrStoreTest, Emplace)
{
    PODPtrStore<int> store;

    EXPECT_EQ(0,store.UsedSlots());

    auto a = store.Emplace(42);
    EXPECT_EQ(1,store.UsedSlots());
    EXPECT_EQ(42,*a);
}

TEST_F(PODPtrStoreTest, DeepCopy)
{
    PODPtrStore<int,2> store0;
    PODPtrStore<int,2> store1;

    EXPECT_EQ(0,store0.UsedSlots());
    EXPECT_EQ(0,store1.UsedSlots());

    auto a = store0.Emplace(123);
    EXPECT_EQ(1,store0.UsedSlots());

    auto mapping = store1.DeepCopy(store0);
    EXPECT_EQ(1,store1.UsedSlots());
    auto b = mapping.begin()->second;
    EXPECT_EQ(123,*b);
}

