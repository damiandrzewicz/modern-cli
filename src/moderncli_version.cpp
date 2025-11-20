#include "moderncli/moderncli.hpp"

namespace moderncli {

static constexpr Version kVersion{
    0,
    1,
    0,
    "0.1.0"
};

const Version& version() noexcept {
    return kVersion;
}

} // namespace moderncli
