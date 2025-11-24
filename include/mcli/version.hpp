
#ifndef MCLI_VERSION_HPP_
#define MCLI_VERSION_HPP_

#include <string_view>

namespace mcli
{

struct Version
{
    int major;
    int minor;
    int patch;
    std::string_view string;
};

[[nodiscard]] const Version& version() noexcept;

}  // namespace mcli

#endif  // MCLI_VERSION_HPP_
