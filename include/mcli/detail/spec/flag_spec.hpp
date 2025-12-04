#ifndef MCLI_DETAIL_SPEC_FLAG_SPEC_HPP_
#define MCLI_DETAIL_SPEC_FLAG_SPEC_HPP_

#include <string>

namespace mcli::detail::spec
{

struct flag_spec
{
    std::string name;  // "--verbose"
    std::string abbr;  // "-v"
    std::string desc;  // help text
};

}  // namespace mcli::detail::spec

#endif  // MCLI_DETAIL_SPEC_FLAG_SPEC_HPP_