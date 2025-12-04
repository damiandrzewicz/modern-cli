#ifndef MCLI_DETAIL_COMMAND_HPP_
#define MCLI_DETAIL_COMMAND_HPP_

#include "mcli/detail/spec/flag_spec.hpp"
#include "mcli/detail/spec/option_spec.hpp"

#include <optional>
#include <stdexcept>
#include <vector>

namespace mcli::detail
{

class command
{
public:
    command() = default;

    void add_flag(spec::flag_spec flag, bool& target)
    {
        spec::option_spec opt;
        opt.name = std::move(flag.name);
        opt.abbr = std::move(flag.abbr);
        opt.desc = std::move(flag.desc);
        opt.target = &target;
        opt.kind = spec::option_kind::flag;
        opt.vkind = spec::value_kind::boolean;

        append_option(std::move(opt));
    }

    std::optional<std::reference_wrapper<spec::option_spec>>
    find_option_by_name(std::string_view name)
    {
        for (auto& opt : m_options)
        {
            if (opt.name == name)
            {
                return std::ref(opt);
            }
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<spec::option_spec>>
    find_option_by_abbr(std::string_view abbr)
    {
        for (auto& opt : m_options)
        {
            if (opt.abbr == abbr)
            {
                return std::ref(opt);
            }
        }
        return std::nullopt;
    }

    void reset_seen_flags()
    {
        for (auto& opt : m_options)
        {
            opt.seen = false;
        }
    }

    // Read-only access to defined options for diagnostics
    const std::vector<spec::option_spec>& options() const noexcept
    {
        return m_options;
    }

private:
    void append_option(spec::option_spec opt)
    {
        assert_option_unique(opt);
        validate(opt);
        m_options.push_back(std::move(opt));
    }

    void assert_option_unique(const spec::option_spec& new_opt)
    {
        for (const auto& opt : m_options)
        {
            if (opt.name == new_opt.name)
            {
                throw std::invalid_argument(
                        std::string{"duplicate option name: "} +
                        std::string{new_opt.name} +
                        std::string{
                                " (names must be unique within a command)"});
            }

            if (!new_opt.abbr.empty() && !opt.abbr.empty())
            {
                if (opt.abbr == new_opt.abbr)
                {
                    throw std::invalid_argument(
                            std::string{"duplicate option abbreviation: "} +
                            std::string{new_opt.abbr} +
                            std::string{" (abbreviations must be unique within "
                                        "a command)"});
                }
            }
        }
    }

    static void validate(const spec::option_spec& opt)
    {
        if (opt.name.empty())
        {
            throw std::invalid_argument("option must have a name");
        }

        if (opt.desc.empty())
        {
            throw std::invalid_argument("option should have help text");
        }
    }

    std::vector<spec::option_spec> m_options;
};

}  // namespace mcli::detail

#endif  // MCLI_DETAIL_COMMAND_HPP_