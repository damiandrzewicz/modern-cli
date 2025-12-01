#ifndef MCLI_DETAIL_BUILDER_FLAG_HPP_
#define MCLI_DETAIL_BUILDER_FLAG_HPP_

// #include "mcli/detail/command.hpp"
#include "mcli/detail/spec/option_spec.hpp"

#include <functional>

namespace mcli::detail::builder
{

namespace
{

inline std::string normalize_long_name(std::string_view name)
{
    if (name.empty())
    {
        return {};
    }

    if (name.rfind("--", 0) == 0)  // already starts with "--"
    {
        return std::string{name};
    }

    // optional: allow a single '-' and upgrade to '--'?
    // if (name.front() == '-' && name.size() > 1 && name[1] != '-')
    //     return std::string{"--"} + std::string{name.substr(1)};

    return std::string{"--"} + std::string{name};
}

inline std::string normalize_short_name(std::string_view abbr)
{
    if (abbr.empty())
    {
        return {};
    }

    if (abbr.rfind('-', 0) == 0)  // already starts with '-'
    {
        // optional: reject "--" here since that's a long option shape
        return std::string{abbr};
    }

    return std::string{"-"} + std::string{abbr};
}

}  // namespace

class command_builder;

class flag_builder
{
public:
    flag_builder(command_builder& parent, mcli::detail::spec::option_spec& opt)
        : m_parent{parent}, m_opt{opt}
    {
    }

    flag_builder& name(std::string_view name)
    {
        auto& opt = m_opt.get();
        opt.name = normalize_long_name(name);
        return *this;
    }

    flag_builder& abbr(std::string_view abbr)
    {
        auto& opt = m_opt.get();
        opt.abbr = normalize_short_name(abbr);
        return *this;
    }

    flag_builder& help(std::string_view help)
    {
        m_opt.get().desc.assign(help.begin(), help.end());
        return *this;
    }

    command_builder& bind(bool& target)
    {
        m_opt.get().target = &target;
        return m_parent.get();
    }

private:
    std::reference_wrapper<command_builder> m_parent;
    std::reference_wrapper<mcli::detail::spec::option_spec> m_opt;
};

}  // namespace mcli::detail::builder

#endif  // MCLI_DETAIL_BUILDER_FLAG_HPP_