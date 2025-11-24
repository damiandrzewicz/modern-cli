#ifndef MCLI_MCLI_HPP
#define MCLI_MCLI_HPP

#include "mcli/detail/config.hpp"
#include "mcli/version.hpp"

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace mcli
{

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

}  // namespace mcli

#endif  // MCLI_MCLI_HPP
