
#include "gtest/gtest.h"
#include "ZLibWrapper.h"

using namespace testing;

class ZLibTest : public Test
{
protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};


TEST_F(ZLibTest, CompressAndDecompress)
{
    std::string s0 = "1234123412341234 This is a Test 1234 1234";
    auto s1 = ZLib::CompressString(s0);
    auto s2 = ZLib::DecompressString(s1);
    EXPECT_EQ(s0,s2);
    SUCCEED() << "Input: " << s0.size() << "; Compressed: " << s1.size();
}


