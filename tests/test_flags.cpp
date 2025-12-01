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
    EXPECT_EQ("Unknown option: --unknown", std::string{result.error_message()});
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
    EXPECT_EQ("Duplicate option: --dry-run",
              std::string{result.error_message()});
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
    EXPECT_EQ("Unknown option: -x", std::string{result.error_message()});
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
    EXPECT_EQ("Duplicate option: -n", std::string{result.error_message()});
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
    // message uses the token that caused duplicate detection
    EXPECT_EQ("Duplicate option: -v", std::string{result.error_message()});
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

TEST(Flags, NormalizesLongNameWithoutDashes)
{
    Options opts;

    // clang-format off
    auto cli = define()
        .flag()
            .name("verbose")           // should normalize to "--verbose"
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
}

TEST(Flags, NormalizesShortNameWithoutDash)
{
    Options opts;

    // clang-format off
    auto cli = define()
        .flag()
            .name("--verbose")
            .abbr("v")                 // should normalize to "-v"
            .help("Enable verbose logging")
            .bind(opts.verbose)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "-v"});

    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_TRUE(result.error_message().empty());
    EXPECT_TRUE(opts.verbose);
}

TEST(Flags, NormalizesLongAndShortNamesTogether)
{
    Options opts;

    // clang-format off
    auto cli = define()
        .flag()
            .name("verbose")   // "verbose" -> "--verbose"
            .abbr("v")         // "v" -> "-v"
            .help("Enable verbose logging")
            .bind(opts.verbose)
        .build();
    // clang-format on

    {
        const auto [argc, argv] = make_argv({"app", "--verbose"});
        auto result = cli.parse(argc, argv);
        ASSERT_TRUE(result);
        EXPECT_TRUE(opts.verbose);
    }

    // reset and try short form
    opts.verbose = false;
    {
        const auto [argc, argv] = make_argv({"app", "-v"});
        auto result = cli.parse(argc, argv);
        ASSERT_TRUE(result);
        EXPECT_TRUE(opts.verbose);
    }
}