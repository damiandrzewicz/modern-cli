#include "mcli/mcli.hpp"
#include "mcli/version.hpp"  // generated

namespace mcli
{

static constexpr Version kVersion{version_major, version_minor, version_patch, version_string};

const Version& version() noexcept
{
    return kVersion;
}

}  // namespace mcli
