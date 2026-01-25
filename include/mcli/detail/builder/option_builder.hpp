#ifndef MCLI_DETAIL_BUILDER_OPTION_BUILDER_HPP_
#define MCLI_DETAIL_BUILDER_OPTION_BUILDER_HPP_

#include "mcli/detail/command.hpp"
#include "mcli/detail/spec/option_spec.hpp"

#include <functional>

namespace mcli::detail::builder
{

class command_builder;

class option_builder
{
public:
    option_builder(command_builder& parent, mcli::detail::command& cmd)
        : m_parent{parent}, m_cmd{cmd}
    {
    }

protected:
    void validate() const
    {
        const auto& name = m_opt.name;
        const auto& abbr = m_opt.abbr;
        const auto& desc = m_opt.desc;

        // validate name

        if (name.empty())
        {
            std::string msg{"option must have a name"};
            if (!abbr.empty())
            {
                msg += " (provided abbreviation \"";
                msg += std::string{abbr};
                msg += "\" without a long name)";
            }
            if (!desc.empty())
            {
                msg += "; help: \"";
                msg += std::string{desc};
                msg += "\"";
            }
            throw std::invalid_argument(msg);
        }

        if (!name.starts_with("--") || name.size() == 2)
        {
            throw std::invalid_argument(
                    "flag name must start with \"--\" followed "
                    "by a non-dash character");
        }

        if (name.size() >= 3 && name[2] == '-')
        {
            throw std::invalid_argument(
                    "flag name cannot start with more than 2 \"-\"");
        }

        // validate abbreveviation

        if (!abbr.empty())
        {
            if (!abbr.starts_with("-") || abbr.size() == 1)
            {
                throw std::invalid_argument(
                        "flag abbreviation must start with \"-\" "
                        "followed by a non-dash character");
            }
            if (abbr.size() >= 2 && abbr[1] == '-')
            {
                throw std::invalid_argument(
                        "flag abbreviation cannot start with more than 1 "
                        "\"-\"");
            }
        }

        if (desc.empty())
        {
            std::string msg{"option "};
            msg += '"';
            msg += std::string{name};
            msg += '"';
            if (!abbr.empty())
            {
                msg += "/";
                msg += std::string{abbr};
            }
            msg += " should have help text";
            throw std::invalid_argument(msg);
        }
    }

    std::reference_wrapper<command_builder> m_parent;
    std::reference_wrapper<mcli::detail::command> m_cmd;
    spec::option_spec m_opt;
};

}  // namespace mcli::detail::builder

#endif  // MCLI_DETAIL_BUILDER_OPTION_BUILDER_HPP_