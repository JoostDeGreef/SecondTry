
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
    Matrix<TypeParam> m0(4,2,{1, 2, 3, 4, 5, 6, 7, 8});
    Matrix<TypeParam> m1(2,4,{1, 3, 5, 7, 2, 4, 6, 8});
    auto m2 = m0.Transposed();
    EXPECT_TRUE(m1 == m2) << "m2 = " << m2;
}

TYPED_TEST(MatrixTest, Slice)
{
    Matrix<TypeParam> m0(4,4,{1, 2, 3, 4, 5, 6, 7, 8, 1, 3, 4, 1, 5, 2, 1, 4});
    Matrix<TypeParam> m1(2,2,{6, 7, 3, 4});
    auto m2 = m0.Slice(1,1,2,2);
    EXPECT_TRUE(m1 == m2) << "m2 = " << m2;
}

TYPED_TEST(MatrixTest, Determinant)
{
    EXPECT_FLOAT_EQ(42,Matrix<TypeParam>(1,1,{42}).Determinant());
    EXPECT_FLOAT_EQ(-6,Matrix<TypeParam>(2,2,{1,3,3,3}).Determinant());
    EXPECT_FLOAT_EQ(60,Matrix<TypeParam>(3,3,{1,5,5,5,2,5,5,5,5}).Determinant());
    EXPECT_FLOAT_EQ(-16,Matrix<TypeParam>(4,4,{1,2,3,4,5,6,7,8,1,3,4,1,5,2,1,4}).Determinant());
}

TYPED_TEST(MatrixTest, Adjoint)
{
    {
        Matrix<TypeParam> m0(1,1,{5});
        Matrix<TypeParam> m1 = m0.Adjoint();
        Matrix<TypeParam> m2(1,1,{1});
        EXPECT_TRUE(m1 == m2) << "m1 = " << m1;
    }
    {
        Matrix<TypeParam> m0(2,2,{1, 3, 6, 7});
        Matrix<TypeParam> m1 = m0.Adjoint();
        Matrix<TypeParam> m2(2,2,{7, -3, -6, 1});
        EXPECT_TRUE(m1 == m2) << "m1 = " << m1;
    }
    {
        Matrix<TypeParam> m0(3,3,{1, 3, 6, 7, 2, 2, 5, 4, 1});
        Matrix<TypeParam> m1 = m0.Adjoint();
        Matrix<TypeParam> m2(3,3,{-6, 21, -6, 3, -29, 40, 18, 11, -19});
        EXPECT_TRUE(m1 == m2) << "m1 = " << m1;
    }
    {
        Matrix<TypeParam> m0(4,4,{1, 2, 3, 4, 5, 6, 7, 8, 1, 3, 4, 1, 5, 2, 1, 4});
        Matrix<TypeParam> m1 = m0.Adjoint();
        Matrix<TypeParam> m2(4,4,{-20, 20, -16, -16, 70, -58, 40, 36, -48, 40, -32, -24, 2, -6, 8, 4});
        EXPECT_TRUE(m1 == m2) << "m1 = " << m1;
    }
}

TYPED_TEST(MatrixTest, Inverse)
{
    Matrix<TypeParam> m0(2,2);
    m0.Set({1,2,3,4});
    Matrix<TypeParam> m1 = m0.Inverse();
    m0 = {-2, 1, 1.5, -0.5};
    EXPECT_TRUE(m0 == m1) << "m1 = " << m1;
    m0 = Matrix<TypeParam>(3,3,{1,2,3, 1,3,4, 5,1,4});
    EXPECT_FLOAT_EQ(1,(m0 *= m0.Inverse()).Determinant()) << "inv(3x3) failed";
    m0 = Matrix<TypeParam>(4,4,{1,2,3,4,5,6,7,8,1,3,4,1,5,2,1,4});
    EXPECT_FLOAT_EQ(1,(m0 *= m0.Inverse()).Determinant()) << "inv(4x4) failed";
}
