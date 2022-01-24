#include "gtest/gtest.h"
#include "Core.h"

using namespace std;
using namespace testing;
using namespace Core;

class SmartPtrStoreTest : public Test
{
protected:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(SmartPtrStoreTest, BasicFunctionality)
{
    SmartPtrStore<int> store;

    EXPECT_EQ(0,store.UniqueObjects());

    auto a = store.Create();
    EXPECT_EQ(1,store.UniqueObjects());

    auto b = a;
    EXPECT_EQ(1,store.UniqueObjects());
    EXPECT_EQ(a,b);

    auto c = a.CreateView();
    EXPECT_EQ(1,store.UniqueObjects());
    EXPECT_EQ(b,c);

    a.Release();
    EXPECT_EQ(1,store.UniqueObjects());

    EXPECT_FALSE(a.IsSet());
    EXPECT_TRUE(b.IsSet());
    EXPECT_TRUE(c.IsSet());

    b.Release();
    EXPECT_EQ(0,store.UniqueObjects());
    EXPECT_FALSE(c.IsSet());
}

TEST_F(SmartPtrStoreTest, Emplace)
{
    SmartPtrStore<int> store;

    EXPECT_EQ(0,store.UniqueObjects());

    auto a = store.Create(42);
    ViewedPtr<int> b = a;
    EXPECT_EQ(1,store.UniqueObjects());
    EXPECT_EQ(42,*a);
}

TEST_F(SmartPtrStoreTest, Copy)
{
    SmartPtrStore<int> store0;
    SmartPtrStore<int> store1;

    EXPECT_EQ(0,store0.UniqueObjects());
    EXPECT_EQ(0,store1.UniqueObjects());

    auto a = store0.Create(123);
    EXPECT_EQ(1,store0.UniqueObjects());

    auto mapping = store1.Copy(store0);
    EXPECT_EQ(1,store1.UniqueObjects());
    auto b = mapping.begin()->second;
    EXPECT_EQ(123,*b);
}

// todo: Copy with limbo objects present
