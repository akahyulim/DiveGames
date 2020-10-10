#include "GoogleTest/include/gtest/gtest.h"

int multiply(int a, int b)
{
	return a * b;
}

int myMax(int a, int b, int c)
{
	if (a >= b && a >= c)
		return a;
	else if (b >= a && b >= c)
		return b;
	return c;
}

TEST(Multiply, TwoMultiplyThree)
{
	EXPECT_EQ(multiply(2, 3), 6);
}

TEST(Multiply, MultiplyZero)
{
	EXPECT_EQ(multiply(2, 0), 0);
	EXPECT_EQ(multiply(-1, 0), 0);
	EXPECT_EQ(multiply(0, 0), 0);
}

TEST(Max, PositiveMax)
{
	ASSERT_EQ(myMax(1, 3, 2), 3);
	ASSERT_EQ(myMax(2, 3, 2), 3);
	ASSERT_EQ(myMax(5, 5, 5), 5);
}