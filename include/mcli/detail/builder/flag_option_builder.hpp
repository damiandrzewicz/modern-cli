#ifndef MCLI_DETAIL_BUILDER_FLAG_OPTION_BUILDER_HPP_
#define MCLI_DETAIL_BUILDER_FLAG_OPTION_BUILDER_HPP_

#include "mcli/detail/builder/option_builder.hpp"

namespace mcli::detail::builder
{

class flag_option_builder : public option_builder
{
public:
    flag_option_builder(command_builder& parent, mcli::detail::command& cmd)
        : option_builder{parent, cmd}
    {
    }

    flag_option_builder& name(std::string_view name)
    {
        m_opt.name.assign(name.begin(), name.end());
        return *this;
    }

    flag_option_builder& abbr(std::string_view abbr)
    {
        m_opt.abbr.assign(abbr.begin(), abbr.end());
        return *this;
    }

    flag_option_builder& help(std::string_view help)
    {
        m_opt.desc.assign(help.begin(), help.end());
        return *this;
    }

    command_builder& bind(bool& target)
    {
        validate();

        m_opt.kind = spec::option_kind::flag;
        m_opt.vkind = spec::value_kind::boolean;
        m_opt.target = &target;

        m_cmd.get().add_option(std::move(m_opt));
        return m_parent.get();
    }
};

}  // namespace mcli::detail::builder

#endif  // MCLI_DETAIL_BUILDER_FLAG_OPTION_BUILDER_HPP_