#include "gtest/gtest.h"

#include "decimal.h"

TEST(Decimal, add1) {
    Decimal d1(-5, 4);
    Decimal d2(-4, 3);

    Decimal d3 = d1 + d2;

    ASSERT_EQ(d3.String(), "-0.0045");
}

TEST(Decimal, sub1) {
    Decimal d1(5, 4);
    Decimal d2(4, 3);

    Decimal d3 = d2 - d1;

    ASSERT_EQ(d3.String(), "0.0035");
}

TEST(Decimal, sub2) {
    Decimal d1(5, 4);
    Decimal d2(4, 3);

    Decimal d3 = d1 - d2;

    ASSERT_EQ(d3.String(), "-0.0035");
}

TEST(Decimal, mult1) {
    Decimal d1(2, 4);
    Decimal d2(4, -3);

    Decimal d3 = d1 * d2;

    ASSERT_EQ(d3.String(), "0.8");
}

TEST(Decimal, div1) {
    Decimal d1(1, 2);
    Decimal d2(2, 0);

    Decimal d3 = d1 / d2;

    ASSERT_EQ(d3.String(), "0.005");
}

TEST(Decimal, string1) {
    Decimal d(4, -3);

    ASSERT_EQ(d.String(), "4000");
}
