#ifndef MCLI_DETAIL_COMMAND_HPP_
#define MCLI_DETAIL_COMMAND_HPP_

#include "mcli/detail/spec/option_spec.hpp"

#include <optional>
#include <vector>

namespace mcli::detail
{

class command
{
public:
    command() = default;

    spec::option_spec& add_flag()
    {
        spec::option_spec opt;
        opt.kind = spec::option_kind::flag;
        opt.vkind = spec::value_kind::boolean;
        m_options.push_back(std::move(opt));
        return m_options.back();
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
    std::vector<spec::option_spec> m_options;
};

}  // namespace mcli::detail

#endif  // MCLI_DETAIL_COMMAND_HPP_