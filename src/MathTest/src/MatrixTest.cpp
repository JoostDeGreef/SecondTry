
#include "gtest/gtest.h"

#include "Matrix.h"

using namespace testing;

class MatrixTest : public Test
{
protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(MatrixTest, Copy)
{
    Matrix<double> m0(5,3,1);
    EXPECT_FLOAT_EQ(1.0,m0(1,4));
    Matrix<double> m1(m0);
    EXPECT_FLOAT_EQ(1.0,m0(1,4));
    EXPECT_FLOAT_EQ(1.0,m1(1,4));
    m1(1,4) = 2;
    EXPECT_FLOAT_EQ(1.0,m0(1,4));
    EXPECT_FLOAT_EQ(2.0,m1(1,4));
}

TEST_F(MatrixTest, Add)
{
    Matrix<double> m0(5,3,1);
    EXPECT_FLOAT_EQ(1.0,m0(1,4));
    Matrix<double> m1(m0);
    EXPECT_FLOAT_EQ(1.0,m0(1,4));
    EXPECT_FLOAT_EQ(1.0,m1(1,4));
    m1 += m0;
    EXPECT_FLOAT_EQ(1.0,m0(1,4));
    EXPECT_FLOAT_EQ(2.0,m1(1,4));
}

TEST_F(MatrixTest, Sum)
{
    Matrix<double> m0(5,3,1);
    EXPECT_FLOAT_EQ(1.0,m0(1,4));
    Matrix<double> m1(m0);
    EXPECT_FLOAT_EQ(1.0,m0(1,4));
    EXPECT_FLOAT_EQ(1.0,m1(1,4));
    auto m2 = m1 + m0;
    EXPECT_FLOAT_EQ(1.0,m0(1,4));
    EXPECT_FLOAT_EQ(1.0,m1(1,4));
    EXPECT_FLOAT_EQ(2.0,m2(1,4));
}
