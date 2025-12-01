#ifndef MCLI_DETAIL_SPEC_OPTION_HPP_
#define MCLI_DETAIL_SPEC_OPTION_HPP_

#include <string>
#include <variant>

namespace mcli::detail::spec
{

enum class option_kind
{
    flag
};

enum class value_kind
{
    boolean
};

struct option_spec
{
    using Target = std::variant<std::monostate, bool*>;

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