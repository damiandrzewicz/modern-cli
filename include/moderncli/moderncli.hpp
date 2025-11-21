#ifndef MODERNCLI_MODERNCLI_HPP
#define MODERNCLI_MODERNCLI_HPP

#include "moderncli/detail/config.hpp"

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace moderncli
{

struct Version
{
    int major;
    int minor;
    int patch;
    std::string_view string;
};

// Will be defined in moderncli_version.cpp
[[nodiscard]] const Version& version() noexcept;

// Placeholder main API (we'll expand later)
class Cli
{
public:
    Cli() = default;

    // Later: define() with constexpr lambdas, args, flags, etc.
    [[nodiscard]] int parse(int argc, char** argv)
    {
        (void) argc;
        (void) argv;
        return 0;
    }
};

}  // namespace moderncli

#endif  // MODERNCLI_MODERNCLI_HPP
