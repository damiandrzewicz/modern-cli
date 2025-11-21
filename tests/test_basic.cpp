#include "moderncli/moderncli.hpp"

#include <gtest/gtest.h>

const int& return_local_ref()
{
    int x = 42;
    return x;  // bugprone-returning-address-of-local
}

TEST(Basic, Version)
{
    const auto& v = moderncli::version();
    EXPECT_EQ(v.major, 0);
    EXPECT_EQ(v.minor, 1);
    EXPECT_EQ(v.patch, 0);
    EXPECT_STREQ(v.string.data(), "0.1.0");
}

TEST(Basic, EmptyParse)
{
    moderncli::Cli cli;
    int argc = 1;
    const char* argv_raw[] = {"moderncli_test"};
    char** argv = const_cast<char**>(argv_raw);

    int rc = cli.parse(argc, argv);
    EXPECT_EQ(rc, 0);
}
