#include "mcli/version.hpp"

#include <gtest/gtest.h>

TEST(Basic, Version)
{
    const mcli::Version v{1, 2, 3, "1.2.3"};

    EXPECT_EQ(v.major, 1);
    EXPECT_EQ(v.minor, 2);
    EXPECT_EQ(v.patch, 3);
    EXPECT_STREQ(v.string.data(), "1.2.3");
}