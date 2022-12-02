
#include "gtest/gtest.h"

#include "Matrix.h"

using namespace testing;

template <typename T>
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

using MatrixFieldTypes = ::testing::Types<float, double>;
TYPED_TEST_SUITE(MatrixTest, MatrixFieldTypes);

TYPED_TEST(MatrixTest, Constructor)
{
    Matrix<TypeParam> m0(5,3);
    ASSERT_EQ(5,m0.Rows());
    ASSERT_EQ(3,m0.Columns());
    Matrix<TypeParam> m1(4,7,1);
    ASSERT_EQ(4,m1.Rows());
    ASSERT_EQ(7,m1.Columns());
    EXPECT_FLOAT_EQ(1.0,m1(1,2));
}

TYPED_TEST(MatrixTest, CopyConstructor)
{
    Matrix<TypeParam> m0(5,3,1);
    Matrix<TypeParam> m1(m0);
    ASSERT_EQ(5,m1.Rows());
    ASSERT_EQ(3,m1.Columns());
    EXPECT_FLOAT_EQ(1.0,m0(1,2));
    EXPECT_FLOAT_EQ(1.0,m1(1,2));
    m1(1,2) = 2;
    EXPECT_FLOAT_EQ(1.0,m0(1,2));
    EXPECT_FLOAT_EQ(2.0,m1(1,2));
}

TYPED_TEST(MatrixTest, GetSet)
{
    // TODO
}

TYPED_TEST(MatrixTest, Add)
{
    Matrix<TypeParam> m0(5,3,1);
    EXPECT_FLOAT_EQ(1.0,m0(1,2));
    Matrix<TypeParam> m1(m0);
    EXPECT_FLOAT_EQ(1.0,m0(1,2));
    EXPECT_FLOAT_EQ(1.0,m1(1,2));
    m1 += m0;
    EXPECT_FLOAT_EQ(1.0,m0(1,2));
    EXPECT_FLOAT_EQ(2.0,m1(1,2));
    m1 += 8;
    EXPECT_FLOAT_EQ(10.0,m1(1,2));
}

TYPED_TEST(MatrixTest, Sum)
{
    Matrix<TypeParam> m0(5,3,1);
    EXPECT_FLOAT_EQ(1.0,m0(1,2));
    Matrix<TypeParam> m1(m0);
    EXPECT_FLOAT_EQ(1.0,m0(1,2));
    EXPECT_FLOAT_EQ(1.0,m1(1,2));
    auto m2 = m1 + m0;
    EXPECT_FLOAT_EQ(1.0,m0(1,2));
    EXPECT_FLOAT_EQ(1.0,m1(1,2));
    EXPECT_FLOAT_EQ(2.0,m2(1,2));
}

TYPED_TEST(MatrixTest, Subtract)
{
    // TODO
}

TYPED_TEST(MatrixTest, Minus)
{
    // TODO
}

TYPED_TEST(MatrixTest, Multiply)
{
    Matrix<TypeParam> m0(2,2,1);
    Matrix<TypeParam> m1(m0);
    m0(0,1) = 2;
    m1(0,0) = 3;
    auto m2 = m0 * m1;
    EXPECT_FLOAT_EQ(5.0,m2(0,0));
    EXPECT_FLOAT_EQ(3.0,m2(0,1));
    EXPECT_FLOAT_EQ(4.0,m2(1,0));
    EXPECT_FLOAT_EQ(2.0,m2(1,1));
}

TYPED_TEST(MatrixTest, Transpose)
{
    // TODO
}

TYPED_TEST(MatrixTest, Determinant)
{
    Matrix<TypeParam> m0(1,1,42);
    EXPECT_FLOAT_EQ(42,m0.Determinant());
    Matrix<TypeParam> m1(2,2,3);
    m1(0,0) = 1;
    EXPECT_FLOAT_EQ(-6,m1.Determinant());
    Matrix<TypeParam> m2(3,3,5);
    m2(0,0) = 1;
    m2(1,1) = 2;
    EXPECT_FLOAT_EQ(60,m2.Determinant());
}

TYPED_TEST(MatrixTest, Inverse)
{
    Matrix<TypeParam> m0(2,2);
    m0(0,0) = 1;
    m0(0,1) = 2;
    m0(1,0) = 3;
    m0(1,1) = 4;
    Matrix<TypeParam> m1 = m0.Inverse();
    EXPECT_FLOAT_EQ(-2,  m1(0,0));
    EXPECT_FLOAT_EQ( 1,  m1(0,1));
    EXPECT_FLOAT_EQ( 1.5,m1(1,0));
    EXPECT_FLOAT_EQ(-0.5,m1(1,1));
}
