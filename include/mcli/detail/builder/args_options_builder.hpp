#ifndef MCLI_DETAIL_BUILDER_ARGS_OPTIONS_BUILDER_HPP_
#define MCLI_DETAIL_BUILDER_ARGS_OPTIONS_BUILDER_HPP_

#include "mcli/detail/builder/option_builder.hpp"

namespace mcli::detail::builder
{

class args_options_builder : public option_builder
{
public:
    args_options_builder(command_builder& parent, mcli::detail::command& cmd)
        : option_builder{parent, cmd}
    {
    }

    args_options_builder& name(std::string_view name)
    {
        m_opt.name.assign(name.begin(), name.end());
        return *this;
    }

    args_options_builder& abbr(std::string_view abbr)
    {
        m_opt.abbr.assign(abbr.begin(), abbr.end());
        return *this;
    }

    args_options_builder& help(std::string_view help)
    {
        m_opt.desc.assign(help.begin(), help.end());
        return *this;
    }

    template <typename T>
    command_builder& bind(T& target)
    {
        validate();

        m_opt.kind = spec::option_kind::arg;

        if constexpr (std::is_same_v<T, std::string>)
        {
            m_opt.vkind = spec::value_kind::string;
        }
        else if constexpr (std::is_integral_v<T>)
        {
            m_opt.vkind = spec::value_kind::integer;
        }
        else
        {
            throw std::invalid_argument(
                    "unsupported argument target type for option: " +
                    m_opt.name);
        }

        m_opt.target = &target;

        m_cmd.get().add_option(std::move(m_opt));
        return m_parent.get();
    }
};

}  // namespace mcli::detail::builder

#endif  // MCLI_DETAIL_BUILDER_ARGS_OPTIONS_BUILDER_HPP_
