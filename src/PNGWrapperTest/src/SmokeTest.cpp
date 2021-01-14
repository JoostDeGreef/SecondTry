
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


TEST_F(PNGTest, PixelProxy)
{
    RGBAImage::Pixel p1(1, 2, 3, 4);
    RGBAImage image(1, 4);
    image[0] = p1;
    image[1].Set(1,2,3,4);
    image[2] = image[1];
    auto& p2 = image[3];
    p2.Set(1, 2, 3, 4);
    EXPECT_EQ(image[0], image[1]);
    EXPECT_EQ(image[1], image[2]);
    EXPECT_EQ(image[2], image[3]);
}

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
    EXPECT_EQ(image, recovered);
}

