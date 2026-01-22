#ifndef MCLI_DETAIL_SPEC_OPTION_HPP_
#define MCLI_DETAIL_SPEC_OPTION_HPP_

#include <cstdint>
#include <string>
#include <variant>

#include <sys/types.h>

namespace mcli::detail::spec
{

enum class option_kind
{
    flag,
    arg
};

enum class value_kind
{
    boolean,
    string,
    integer
};

struct option_spec
{
    using Target = std::variant<std::monostate,
                                bool*,
                                std::string*,
                                int64_t*,
                                uint64_t*,
                                int32_t*,
                                uint32_t*,
                                uint16_t*,
                                int16_t*,
                                uint8_t*,
                                int8_t*>;

    std::string name;  // Full name, e.g. "verbose"
    std::string abbr;  // Abbreviation, e.g. "v"
    std::string desc;  // Help text

    option_kind kind{option_kind::flag};
    Target target{std::monostate{}};
    value_kind vkind{value_kind::boolean};
    bool seen{false};
};

}  // namespace mcli::detail::spec

#endif  // MCLI_DETAIL_SPEC_OPTION_HPP_