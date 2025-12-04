#ifndef MCLI_DETAIL_BUILDER_COMMAND_BUILDER_HPP_
#define MCLI_DETAIL_BUILDER_COMMAND_BUILDER_HPP_

#include "mcli/detail/builder/flag_builder.hpp"
#include "mcli/detail/command.hpp"
#include "mcli/detail/parse/command_parser.hpp"

namespace mcli::detail::builder
{

class command_builder
{
public:
    command_builder() = default;

    /**
     * @brief Start building a flag option.
     */
    flag_builder flag()
    {
        return flag_builder{*this, m_cmd};
    }

    /**
     * @brief Finalize and build the command.
     */
    [[nodiscard]] parse::command_parser build()
    {
        return parse::command_parser{std::move(m_cmd)};
    }

private:
    mcli::detail::command m_cmd;
};

}  // namespace mcli::detail::builder

#endif  // MCLI_DETAIL_BUILDER_COMMAND_BUILDER_HPP_