
#include "gtest/gtest.h"
#include "PNGWrapper.h"

using namespace testing;

class PNGTest : public Test
{
protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};


TEST_F(PNGTest, StoreAndRead)
{
    RGBAImage::Pixel p(255, 2, 0, 255);
    RGBAImage image(2, 2);
    image[0] = p;
    image[1] = p;
    image[2] = p;
    image[3] = p;
    auto stored = image.StorePNG();
    SUCCEED() << "png size: " << stored.size();
    RGBAImage recovered(stored);
    EXPECT_EQ(image,recovered);
}


