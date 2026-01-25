#include "mcli/mcli.hpp"
#include "utils/args_builder.hpp"

#include <cstdint>

#include <gtest/gtest.h>

using test::utils::make_argv;

namespace
{

struct Opts
{
    std::int64_t port = 0;
    std::string name;
};

struct StringPort
{
    std::string port;
};

struct NumericTargets
{
    std::uint16_t port_u16 = 0;
    std::uint64_t port_u64 = 0;
};

}  // namespace

TEST(ArgsOptions, LongFormSpaceSeparated)
{
    Opts opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port to bind")
            .bind(opts.port)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--port", "8080"});
    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_EQ(opts.port, 8080);
}

TEST(ArgsOptions, LongFormEquals)
{
    Opts opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port to bind")
            .bind(opts.port)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--port=8080"});
    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_EQ(opts.port, 8080);
}

TEST(ArgsOptions, ShortFormSpaceSeparated)
{
    Opts opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port to bind")
            .bind(opts.port)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "-p", "8080"});
    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_EQ(opts.port, 8080);
}

TEST(ArgsOptions, MissingValue)
{
    Opts opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port to bind")
            .bind(opts.port)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--port"});
    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(),
              mcli::detail::parse::parse_error::missing_value);
    EXPECT_NE(std::string{result.error_message()}.find(
                      "missing value for option: --port"),
              std::string::npos);
}

TEST(ArgsOptions, InvalidInteger)
{
    Opts opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port to bind")
            .bind(opts.port)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--port", "abc"});
    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(),
              mcli::detail::parse::parse_error::invalid_value);
    EXPECT_NE(std::string{result.error_message()}.find(
                      "Invalid integer for --port"),
              std::string::npos);
}

TEST(ArgsOptions, DuplicateValueOption)
{
    Opts opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port to bind")
            .bind(opts.port)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--port", "1", "--port", "2"});
    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(),
              mcli::detail::parse::parse_error::duplicate_option);
    EXPECT_NE(std::string{result.error_message()}.find(
                      "Duplicate option: --port"),
              std::string::npos);
}

TEST(ArgsOptions, CompactShortFormRejected)
{
    Opts opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port to bind")
            .bind(opts.port)
        .build();
    // clang-format on

    // -p8080 is not allowed (no POSIX compact)
    const auto [argc, argv] = make_argv({"app", "-p8080"});
    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(),
              mcli::detail::parse::parse_error::unknown_option);
    EXPECT_NE(
            std::string{result.error_message()}.find("Unknown option: -p8080"),
            std::string::npos);
}

TEST(ArgsOptions, StringOption_AssignsRawText)
{
    Opts opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--name")
            .abbr("-n")
            .help("Service name")
            .bind(opts.name)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--name", "alice"});
    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_EQ(opts.name, "alice");
}

TEST(ArgsOptions, StringOption_AllowsNumericText)
{
    Opts opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--name")
            .abbr("-n")
            .help("Service name")
            .bind(opts.name)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--name", "123"});
    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_EQ(opts.name, "123");
}

TEST(ArgsOptions, PortBoundToString_IsString)
{
    StringPort opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port as text")
            .bind(opts.port)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--port", "8080"});
    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_EQ(opts.port, "8080");
}

TEST(ArgsOptions, Unsigned16_ParsesAndAssigns)
{
    NumericTargets opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port")
            .bind(opts.port_u16)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--port", "65535"});
    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_EQ(opts.port_u16, 65535);
}

TEST(ArgsOptions, Unsigned16_RejectsOutOfRange)
{
    NumericTargets opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port")
            .bind(opts.port_u16)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--port", "70000"});
    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(),
              mcli::detail::parse::parse_error::invalid_value);
}

TEST(ArgsOptions, Unsigned16_RejectsNegative)
{
    NumericTargets opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port")
            .bind(opts.port_u16)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--port", "-1"});
    auto result = cli.parse(argc, argv);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error_code(),
              mcli::detail::parse::parse_error::invalid_value);
}

TEST(ArgsOptions, Unsigned64_AllowsLargeValues)
{
    NumericTargets opts;

    // clang-format off
    auto cli = mcli::define()
        .arg()
            .name("--port")
            .abbr("-p")
            .help("Port")
            .bind(opts.port_u64)
        .build();
    // clang-format on

    const auto [argc, argv] = make_argv({"app", "--port", "4294967296"});
    auto result = cli.parse(argc, argv);
    ASSERT_TRUE(result);
    EXPECT_EQ(opts.port_u64, 4294967296ULL);
}
