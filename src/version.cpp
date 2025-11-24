#include "mcli/version.hpp"
#include "mcli/version_gen.hpp"  // generated

namespace mcli
{

static constexpr Version kVersion{
        version_major,
        version_minor,
        version_patch,
        version_string,
};

const Version& version() noexcept
{
    return kVersion;
}

}  // namespace mcli
