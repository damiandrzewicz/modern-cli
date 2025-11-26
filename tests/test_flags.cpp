#include "mcli/core.hpp"
#include "utils/args_builder.hpp"

#include <gtest/gtest.h>

using mcli::define;
using test::utils::make_argv;

namespace
{

struct Options
{
    bool verbose = false;
    bool dry_run = false;
};

}  // namespace

TEST(Flags, DefaultValues)
{
    Options opts;
    auto cli = define(
            [&](auto& cmd)
            {
                cmd.flag({"--verbose", "Enable verbose logging", opts.verbose});
                cmd.flag({"--dry-run", "Do not perform changes", opts.dry_run});
            });

    const auto [argc, argv] = make_argv({"app"});

    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);

    EXPECT_FALSE(opts.verbose);
    EXPECT_FALSE(opts.dry_run);
}

TEST(Flags, SingleFlagSet)
{
    Options opts;
    auto cli = define(
            [&](auto& cmd)
            {
                cmd.flag({"--verbose", "Enable verbose logging", opts.verbose});
                cmd.flag({"--dry-run", "Do not perform changes", opts.dry_run});
            });

    const auto [argc, argv] = make_argv({"app", "--verbose"});

    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_TRUE(result.error_message().empty());

    EXPECT_TRUE(opts.verbose);
    EXPECT_FALSE(opts.dry_run);
}

TEST(Flags, UnknownFlag)
{
    Options opts;
    auto cli = define(
            [&](auto& cmd)
            {
                cmd.flag({"--verbose", "Enable verbose logging", opts.verbose});
                cmd.flag({"--dry-run", "Do not perform changes", opts.dry_run});
            });

    const auto [argc, argv] = make_argv({"app", "--unknown"});

    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(), mcli::parse_error::unknown_option);
    EXPECT_EQ("Unknown option: --unknown", std::string{result.error_message()});
}

TEST(Flags, DuplicateFlag)
{
    Options opts;
    auto cli = define(
            [&](auto& cmd)
            {
                cmd.flag({"--verbose", "Enable verbose logging", opts.verbose});
                cmd.flag({"--dry-run", "Do not perform changes", opts.dry_run});
            });

    const auto [argc, argv] = make_argv({"app", "--dry-run", "--dry-run"});

    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(), mcli::parse_error::duplicate_option);
    EXPECT_EQ("Duplicate option: --dry-run",
              std::string{result.error_message()});
}