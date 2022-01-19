#include "gtest/gtest.h"
#include "Geometry.h"

using namespace std;
using namespace testing;
using namespace Geometry;

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
    PODPtrStore<std::string> store;

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
    PODPtrStore<std::string> store;

    EXPECT_EQ(0,store.UsedSlots());

    auto a = store.Emplace("test");
    EXPECT_EQ(1,store.UsedSlots());
    EXPECT_STREQ("test",a->c_str());
}

// TODO: test DeepCopy