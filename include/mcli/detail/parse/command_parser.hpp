#ifndef MCLI_DETAIL_PARSE_COMMAND_PARSER_HPP_
#define MCLI_DETAIL_PARSE_COMMAND_PARSER_HPP_

#include "mcli/detail/command.hpp"
#include "mcli/detail/parse/parse_result.hpp"
#include "mcli/detail/spec/option_spec.hpp"

#include <cassert>
#include <span>

namespace mcli::detail::parse
{

class command_parser
{
public:
    command_parser(mcli::detail::command&& cmd) : m_cmd{std::move(cmd)} {}

    [[nodiscard]] parse_result parse(int argc, char** argv)
    {
        parse_result result = parse_result::success();

        reset_seen_flags();
        parse_range(argc, argv, 1, result);

        return result;
    }

private:
    void reset_seen_flags()
    {
        m_cmd.reset_seen_flags();
    }

    void parse_range(int argc,
                     char** argv,
                     int start_index,
                     parse_result& result)
    {
        std::span<char*> args(argv, static_cast<std::size_t>(argc));
        for (char* arg : args.subspan(static_cast<std::size_t>(start_index)))
        {
            std::string_view tok{arg};
            if (!handle_token(tok, result))
            {
                return;
            }
        }
    }

    bool handle_token(std::string_view tok, parse_result& result)
    {
        if (tok.empty())
        {
            return true;
        }

        auto option = m_cmd.find_option_by_name(tok);
        if (!option.has_value())
        {
            option = m_cmd.find_option_by_abbr(tok);
        }

        // Option (flag)
        if (!option.has_value())
        {
            result = parse_result::failure(
                    parse_error::unknown_option,
                    "Unknown option: " + std::string{tok});
            return false;
        }

        auto& opt = option->get();
        if (opt.seen)
        {
            result = parse_result::failure(
                    parse_error::duplicate_option,
                    "Duplicate option: " + std::string{tok});
            return false;
        }

        opt.seen = true;

        return apply_option(opt, result);
    }

    static bool apply_option(spec::option_spec& opt, parse_result& /*result*/)
    {
        using namespace mcli::detail::spec;

        switch (opt.kind)
        {
            case option_kind::flag:
            {
                apply_flag(opt);
                return true;
            }
            default:
            {
                assert(false && "unknown option kind");
                return false;
            }
        }
    }

    // Apply flag behavior.
    static void apply_flag(spec::option_spec& opt)
    {
        using namespace mcli::detail::spec;

        if (opt.vkind == value_kind::boolean)
        {
            assert(std::holds_alternative<bool*>(opt.target));
            if (auto* ptr = std::get_if<bool*>(&opt.target))
            {
                assert(*ptr != nullptr);
                **ptr = true;
            }
        }
        // future: other flag types
    }

    mcli::detail::command m_cmd;
};

}  // namespace mcli::detail::parse

#endif  // MCLI_DETAIL_PARSE_COMMAND_PARSER_HPP_