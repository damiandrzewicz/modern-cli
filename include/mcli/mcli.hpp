#ifndef MCLI_MCLI_HPP
#define MCLI_MCLI_HPP

#include "mcli/detail/builder/command_builder.hpp"
#include "mcli/version.hpp"

namespace mcli
{

/**
 * @brief Define a command-line interface.
 */
[[nodiscard]] detail::builder::command_builder define()
{
    return detail::builder::command_builder{};
}

}  // namespace mcli

#endif  // MCLI_MCLI_HPP
