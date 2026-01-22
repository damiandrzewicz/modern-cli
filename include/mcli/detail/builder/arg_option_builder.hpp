#ifndef MCLI_DETAIL_BUILDER_ARG_OPTION_BUILDER_HPP_
#define MCLI_DETAIL_BUILDER_ARG_OPTION_BUILDER_HPP_

// Backward-compat shim for the old header name.
// Prefer including "mcli/detail/builder/args_options_builder.hpp" directly.

#include "mcli/detail/builder/args_options_builder.hpp"

namespace mcli::detail::builder
{

using arg_option_builder [[deprecated(
        "Use args_options_builder from args_options_builder.hpp")]] =
        args_options_builder;

}  // namespace mcli::detail::builder

#endif  // MCLI_DETAIL_BUILDER_ARG_OPTION_BUILDER_HPP_