#ifndef MCLI_DETAIL_BUILDER_FLAG_HPP_
#define MCLI_DETAIL_BUILDER_FLAG_HPP_

#include "mcli/detail/command.hpp"
#include "mcli/detail/spec/flag_spec.hpp"
#include "mcli/detail/spec/option_spec.hpp"

#include <functional>
#include <stdexcept>

namespace mcli::detail::builder
{

class command_builder;

class flag_builder
{
public:
    flag_builder(command_builder& parent, mcli::detail::command& cmd)
        : m_parent{parent}, m_cmd{cmd}
    {
    }

    flag_builder& name(std::string_view name)
    {
        if (name.size() < 3)
        {
            throw std::invalid_argument(
                    "flag name must start with \"--\" followed by a non-dash "
                    "character");
        }

        if (name[0] != '-' || name[1] != '-')
        {
            throw std::invalid_argument("flag name must start with \"--\"");
        }

        if (name[2] == '-')
        {
            throw std::invalid_argument(
                    "flag name cannot start with more than 2 \"-\"");
        }

        m_flag.name.assign(name.begin(), name.end());
        return *this;
    }

    flag_builder& abbr(std::string_view abbr)
    {
        m_flag.abbr.assign(abbr.begin(), abbr.end());
        return *this;
    }

    flag_builder& help(std::string_view help)
    {
        m_flag.desc.assign(help.begin(), help.end());
        return *this;
    }

    command_builder& bind(bool& target)
    {
        if (m_flag.name.empty())
        {
            throw std::invalid_argument(
                    "flag must have a name before binding to a target");
        }

        m_cmd.get().add_flag(std::move(m_flag), target);
        return m_parent.get();
    }

private:
    std::reference_wrapper<command_builder> m_parent;
    std::reference_wrapper<mcli::detail::command> m_cmd;

    spec::flag_spec m_flag;
};

}  // namespace mcli::detail::builder

#endif  // MCLI_DETAIL_BUILDER_FLAG_HPP_