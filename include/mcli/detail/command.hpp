#ifndef MCLI_DETAIL_COMMAND_HPP_
#define MCLI_DETAIL_COMMAND_HPP_

#include "mcli/detail/spec/flag_spec.hpp"
#include "mcli/detail/spec/option_spec.hpp"

#include <cassert>
#include <optional>
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

        assert_option_unique(opt);

        m_options.push_back(std::move(opt));
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

private:
    void assert_option_unique(const spec::option_spec& new_opt)
    {
        for (const auto& opt : m_options)
        {
            assert(opt.name != new_opt.name &&
                   "option name must be unique within command");
            assert(opt.abbr != new_opt.abbr &&
                   "option abbreviation must be unique within command");
        }
    }

    std::vector<spec::option_spec> m_options;
};

}  // namespace mcli::detail

#endif  // MCLI_DETAIL_COMMAND_HPP_