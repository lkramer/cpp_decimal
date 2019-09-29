#include "gtest/gtest.h"

#include <limits>
#include <iostream>

#include "decimal.h"

TEST(Decimal, create1) {
    Decimal d(5, 0);

    ASSERT_EQ(d.String(), "5");
}

TEST(Decimal, create2) {
    Decimal d(-5, 0);

    ASSERT_EQ(d.String(), "-5");
}

TEST(Decimal, create3) {
    Decimal d(5, 1);

    ASSERT_EQ(d.String(), "0.5");
}

TEST(Decimal, create4) {
    Decimal d(-5, 1);

    ASSERT_EQ(d.String(), "-0.5");
}

TEST(Decimal, create5) {
    Decimal d(-5355433567236, 8);

    ASSERT_EQ(d.String(), "-53554.33567236");
}

TEST(Decimal, create_max) {
    Decimal d(std::numeric_limits<int64_t>::max(), 0);

    ASSERT_EQ(d.String(), "9223372036854775807");
}

TEST(Decimal, create_min) {
    Decimal d(std::numeric_limits<int64_t>::min()+1, 0);

    ASSERT_EQ(d.String(), "-9223372036854775807");
}

TEST(Decimal, create_double) {
    double val = 3.14;
    Decimal d(val);

    ASSERT_EQ(d.String(), "3.14");
}

