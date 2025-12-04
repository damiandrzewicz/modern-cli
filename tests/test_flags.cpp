#include "mcli/mcli.hpp"
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

    // clang-format off
    auto cli = define()
        .flag()
            .name("--verbose")
            .abbr("-v")
            .help("Enable verbose logging")
            .bind(opts.verbose)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app"});

    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);

    EXPECT_FALSE(opts.verbose);
    EXPECT_FALSE(opts.dry_run);
}

TEST(Flags, SingleFlagSet)
{
    Options opts;

    // clang-format off
    auto cli = define()
        .flag()
            .name("--verbose")
            .abbr("-v")
            .help("Enable verbose logging")
            .bind(opts.verbose)
        .build();
    // clang-format on

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

    // clang-format off
    auto cli = define()
        .flag()
            .name("--verbose")
            .abbr("-v")
            .help("Enable verbose logging")
            .bind(opts.verbose)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--unknown"});

    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(),
              mcli::detail::parse::parse_error::unknown_option);
    const std::string msg_unknown_long = std::string{result.error_message()};
    EXPECT_NE(msg_unknown_long.find("Unknown option: --unknown"),
              std::string::npos);
    EXPECT_NE(msg_unknown_long.find("Available options:"), std::string::npos);
}

TEST(Flags, DuplicateFlag)
{
    Options opts;

    // clang-format off
    auto cli = define()
        .flag()
            .name("--dry-run")
            .abbr("-n")
            .help("Do not perform changes")
            .bind(opts.dry_run)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--dry-run", "--dry-run"});

    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(),
              mcli::detail::parse::parse_error::duplicate_option);
    const std::string msg_dup_long = std::string{result.error_message()};
    EXPECT_NE(msg_dup_long.find("Duplicate option: --dry-run"),
              std::string::npos);
    EXPECT_NE(msg_dup_long.find("(option --dry-run/-n already set)"),
              std::string::npos);
}

TEST(Flags, ShortFlagSetsValue)
{
    Options opts;

    // clang-format off
    auto cli = define()
        .flag()
            .name("--verbose")
            .abbr("-v")
            .help("Enable verbose logging")
            .bind(opts.verbose)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "-v"});

    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_TRUE(opts.verbose);
}

TEST(Flags, UnknownShortFlag)
{
    Options opts;

    // clang-format off
    auto cli = define()
        .flag()
            .name("--verbose")
            .abbr("-v")
            .help("Enable verbose logging")
            .bind(opts.verbose)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "-x"});

    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(),
              mcli::detail::parse::parse_error::unknown_option);
    const std::string msg_unknown_short = std::string{result.error_message()};
    EXPECT_NE(msg_unknown_short.find("Unknown option: -x"), std::string::npos);
    EXPECT_NE(msg_unknown_short.find("Available options:"), std::string::npos);
}

TEST(Flags, DuplicateShortFlag)
{
    Options opts;

    // clang-format off
    auto cli = define()
        .flag()
            .name("--dry-run")
            .abbr("-n")
            .help("Do not perform changes")
            .bind(opts.dry_run)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "-n", "-n"});

    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(),
              mcli::detail::parse::parse_error::duplicate_option);
    const std::string msg_dup_short = std::string{result.error_message()};
    EXPECT_NE(msg_dup_short.find("Duplicate option: -n"), std::string::npos);
    EXPECT_NE(msg_dup_short.find("(option --dry-run/-n already set)"),
              std::string::npos);
}

TEST(Flags, LongAndShortDuplicate)
{
    Options opts;

    // clang-format off
    auto cli = define()
        .flag()
            .name("--verbose")
            .abbr("-v")
            .help("Enable verbose logging")
            .bind(opts.verbose)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--verbose", "-v"});

    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(),
              mcli::detail::parse::parse_error::duplicate_option);
    // message uses the token that caused duplicate detection and includes
    // canonical name
    const std::string msg_dup_mix = std::string{result.error_message()};
    EXPECT_NE(msg_dup_mix.find("Duplicate option: -v"), std::string::npos);
    EXPECT_NE(msg_dup_mix.find("(option --verbose/-v already set)"),
              std::string::npos);
}

TEST(Flags, MultipleFlagsSet)
{
    Options opts;

    // clang-format off
    auto cli = define()
        .flag()
            .name("--verbose")
            .abbr("-v")
            .help("Enable verbose logging")
            .bind(opts.verbose)
        .flag()
            .name("--dry-run")
            .abbr("-n")
            .help("Do not perform changes")
            .bind(opts.dry_run)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--verbose", "--dry-run"});

    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_TRUE(opts.verbose);
    EXPECT_TRUE(opts.dry_run);
}

TEST(Flags, LongNameWithoutDashes_Throws)
{
    Options opts;

    try
    {
        // clang-format off
        auto cli = define()
            .flag()
                .name("DryRun")
                .help("...")
                .bind(opts.verbose)
            .build();
        // clang-format on

        FAIL() << "Expected an exception for invalid long flag name, but none "
                  "was thrown";
    }
    catch (const std::invalid_argument& ex)
    {
        EXPECT_EQ(std::string{ex.what()}, "flag name must start with \"--\"");
    }
    catch (...)
    {
        FAIL() << "Expected std::invalid_argument for invalid long flag name";
    }
}

TEST(Flags, LongNameDashesOnly_Throws)
{
    Options opts;

    try
    {
        // clang-format off
        auto cli = define()
            .flag()
                .name("--")
                .help("...")
                .bind(opts.verbose)
            .build();
        // clang-format on

        FAIL() << "Expected an exception for invalid long flag name, but none "
                  "was thrown";
    }
    catch (const std::invalid_argument& ex)
    {
        EXPECT_EQ(std::string{ex.what()},
                  "flag name must start with \"--\" followed by a non-dash "
                  "character");
    }
    catch (...)
    {
        FAIL() << "Expected std::invalid_argument for invalid long flag name";
    }
}

TEST(Flags, LongNameSingleDash_Throws)
{
    Options opts;

    try
    {
        // clang-format off
        auto cli = define()
            .flag()
                .name("-verbose")
                .help("...")
                .bind(opts.verbose)
            .build();
        // clang-format on

        FAIL() << "Expected an exception for invalid long flag name, but none "
                  "was thrown";
    }
    catch (const std::invalid_argument& ex)
    {
        EXPECT_EQ(std::string{ex.what()}, "flag name must start with \"--\"");
    }
    catch (...)
    {
        FAIL() << "Expected std::invalid_argument for invalid long flag name";
    }
}

TEST(Flags, LongNameLikeAbbr_Throws)
{
    Options opts;

    try
    {
        // clang-format off
        auto cli = define()
            .flag()
                .name("-v")
                .help("...")
                .bind(opts.verbose)
            .build();
        // clang-format on

        FAIL() << "Expected an exception for invalid long flag name, but none "
                  "was thrown";
    }
    catch (const std::invalid_argument& ex)
    {
        EXPECT_EQ(std::string{ex.what()},
                  "flag name must start with \"--\" followed by a non-dash "
                  "character");
    }
    catch (...)
    {
        FAIL() << "Expected std::invalid_argument for invalid long flag name";
    }
}

TEST(Flags, LongNameMultipleDashes_Throws)
{
    Options opts;

    try
    {
        // clang-format off
        auto cli = define()
            .flag()
                .name("---verbose")
                .help("...")
                .bind(opts.verbose)
            .build();
        // clang-format on

        FAIL() << "Expected an exception for invalid long flag name, but none "
                  "was thrown";
    }
    catch (const std::invalid_argument& ex)
    {
        EXPECT_EQ(std::string{ex.what()},
                  "flag name cannot start with more than 2 \"-\"");
    }
    catch (...)
    {
        FAIL() << "Expected std::invalid_argument for invalid long flag name";
    }
}

TEST(Flags, MissingName_Throws)
{
    Options opts;

    try
    {
        // clang-format off
        auto cli = define()
            .flag()
                .help("...")
                .bind(opts.verbose)
            .build();
        // clang-format on

        FAIL() << "Expected an exception for missing long flag name, but none "
                  "was thrown";
    }
    catch (const std::invalid_argument& ex)
    {
        EXPECT_EQ(std::string{ex.what()},
                  "flag must have a name before binding to a target");
    }
    catch (...)
    {
        FAIL() << "Expected std::invalid_argument for missing long flag name";
    }
}

TEST(Flags, MissingHelp_Throws)
{
    Options opts;

    try
    {
        // clang-format off
        auto cli = define()
            .flag()
                .name("--verbose")
                .bind(opts.verbose)
            .build();
        // clang-format on

        FAIL() << "Expected an exception for missing help text, but none was "
                  "thrown";
    }
    catch (const std::invalid_argument& ex)
    {
        EXPECT_EQ(std::string{ex.what()}, "option should have help text");
    }
    catch (...)
    {
        FAIL() << "Expected std::invalid_argument for missing help text";
    }
}