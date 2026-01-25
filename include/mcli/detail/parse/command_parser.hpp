#ifndef MCLI_DETAIL_PARSE_COMMAND_PARSER_HPP_
#define MCLI_DETAIL_PARSE_COMMAND_PARSER_HPP_

#include "mcli/detail/command.hpp"
#include "mcli/detail/parse/parse_result.hpp"
#include "mcli/detail/spec/option_spec.hpp"

#include <cassert>
#include <charconv>
#include <limits>
#include <span>
#include <string>

namespace mcli::detail::parse
{

struct raw_option_value
{
    std::string_view text;
};

class command_parser
{
public:
    explicit command_parser(mcli::detail::command&& cmd) : m_cmd{std::move(cmd)}
    {
    }

    [[nodiscard]] parse_result parse(int argc, char** argv)
    {
        parse_result result = parse_result::success();

        reset_seen_options();
        parse_range(argc, argv, 1, result);

        return result;
    }

private:
    void reset_seen_options()
    {
        m_cmd.reset_seen_options();
    }

    void parse_range(int argc,
                     char** argv,
                     int start_index,
                     parse_result& result)
    {
        std::span<char*> args(argv, static_cast<std::size_t>(argc));
        bool only_positionals = false;

        for (auto idx = static_cast<std::size_t>(start_index);
             idx < args.size();
             ++idx)
        {
            std::string_view tok{args[idx]};

            if (tok == "--")
            {
                only_positionals = true;
                continue;
            }

            if (!only_positionals && looks_like_compact_short_option(tok))
            {
                result = parse_result::failure(
                        parse_error::unknown_option,
                        build_unknown_option_message(tok));
                return;
            }

            if (only_positionals || !is_option_token(tok))
            {
                // Positional parsing is a future feature.
                // For now: accept and ignore positional tokens.
                continue;
            }

            // Option parsing
            if (!handle_option_token(args, idx, tok, result))
            {
                return;
            }
        }
    }

    static bool looks_like_compact_short_option(std::string_view tok)
    {
        // Reject POSIX-style compact forms like -p8080.
        // Allowed short option tokens are exactly "-x".
        return tok.size() > 2 && tok.starts_with("-") && !tok.starts_with("--");
    }

    static bool is_option_token(std::string_view tok)
    {
        if (tok.empty() || tok == "-")
        {
            return false;
        }

        // Long option: --name or --name=value
        if (tok.starts_with("--"))
        {
            return tok.size() > 2;
        }

        // Short option: -x (exactly 2 chars). No clusters, no -p8080.
        if (tok.starts_with("-"))
        {
            return tok.size() == 2;
        }

        return false;
    }

    bool handle_option_token(std::span<char*> args,
                             std::size_t& idx,
                             std::string_view tok,
                             parse_result& result)
    {
        if (tok.empty())
        {
            return true;
        }

        // Split --name=value
        std::string_view key = tok;
        std::string_view value;
        bool has_inline_value = false;

        if (tok.starts_with("--"))
        {
            const auto eq_pos = tok.find('=');
            if (eq_pos != std::string_view::npos)
            {
                key = tok.substr(0, eq_pos);
                value = tok.substr(eq_pos + 1);
                has_inline_value = true;
            }
        }

        auto option = m_cmd.find_option_by_name(key);
        if (!option.has_value())
        {
            option = m_cmd.find_option_by_abbr(key);
        }

        if (!option.has_value())
        {
            result = parse_result::failure(parse_error::unknown_option,
                                           build_unknown_option_message(tok));
            return false;
        }

        auto& opt = option->get();
        if (opt.seen)
        {
            result = parse_result::failure(
                    parse_error::duplicate_option,
                    build_duplicate_option_message(key, opt));
            return false;
        }

        opt.seen = true;

        // Flags take no values
        if (opt.kind == spec::option_kind::flag)
        {
            if (has_inline_value)
            {
                result = parse_result::failure(
                        parse_error::unexpected_value,
                        std::string{"flag does not take a value: "} +
                                std::string{key});
                return false;
            }
            return apply_option(opt, result);
        }

        // Args require a value
        if (opt.kind == spec::option_kind::arg)
        {
            if (!has_inline_value)
            {
                // Expect next token as value
                if (idx + 1 >= args.size())
                {
                    result = parse_result::failure(
                            parse_error::missing_value,
                            std::string{"missing value for option: "} +
                                    std::string{key});
                    return false;
                }

                std::string_view next_tok{args[idx + 1]};
                if (next_tok == "--")
                {
                    result = parse_result::failure(
                            parse_error::missing_value,
                            std::string{"missing value for option: "} +
                                    std::string{key});
                    return false;
                }
                value = next_tok;
                ++idx;  // consume value token
            }

            return apply_arg_value(opt, key, raw_option_value{value}, result);
        }

        assert(false && "unknown option kind");
        return false;
    }

    std::string build_unknown_option_message(std::string_view tok) const
    {
        std::string msg = "Unknown option: ";
        msg += tok;

        // Append available options for better guidance
        const auto& opts = m_cmd.options();
        if (!opts.empty())
        {
            msg += ". Available options: ";
            bool first = true;
            for (const auto& opt_item : opts)
            {
                if (!first)
                {
                    msg += "; ";
                }
                first = false;

                msg += opt_item.name;
                if (!opt_item.abbr.empty())
                {
                    msg += " (";
                    msg += opt_item.abbr;
                    msg += ")";
                }
                if (!opt_item.desc.empty())
                {
                    msg += ": ";
                    msg += opt_item.desc;
                }
            }
        }
        return msg;
    }

    static std::string build_duplicate_option_message(
            std::string_view tok, const spec::option_spec& opt)
    {
        std::string msg = "Duplicate option: ";
        msg += tok;
        msg += " (option ";
        msg += opt.name;
        if (!opt.abbr.empty())
        {
            msg += "/";
            msg += opt.abbr;
        }
        msg += " already set)";
        return msg;
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
            case option_kind::arg:
            {
                // Value options are handled in handle_option_token.
                return true;
            }
            default:
            {
                assert(false && "unknown option kind");
                return false;
            }
        }
    }

    static bool apply_arg_value(spec::option_spec& opt,
                                std::string_view option_key,
                                raw_option_value raw_value,
                                parse_result& result)
    {
        using namespace mcli::detail::spec;

        switch (opt.vkind)
        {
            case value_kind::string:
            {
                if (auto* ptr = std::get_if<std::string*>(&opt.target))
                {
                    assert(*ptr != nullptr);
                    **ptr = std::string{raw_value.text};
                    return true;
                }
                break;
            }
            case value_kind::integer:
            {
                return apply_integer_target(opt, option_key, raw_value, result);
            }
            default:
                break;
        }

        result = parse_result::failure(
                parse_error::invalid_value,
                std::string{"unsupported target type for option: "} +
                        std::string{option_key});
        return false;
    }

    static bool apply_integer_target(spec::option_spec& opt,
                                     std::string_view option_key,
                                     raw_option_value raw_value,
                                     parse_result& result)
    {
        auto fail = [&](std::string msg)
        {
            result = parse_result::failure(parse_error::invalid_value,
                                           std::move(msg));
            return false;
        };

        auto parse_i64 = [&](std::int64_t& out)
        {
            const char* begin = raw_value.text.data();
            const char* end = raw_value.text.data() + raw_value.text.size();
            auto [p, ec] = std::from_chars(begin, end, out);
            return ec == std::errc{} && p == end;
        };

        std::int64_t parsed_value = 0;
        if (!parse_i64(parsed_value))
        {
            std::string msg = "Invalid integer for ";
            msg += option_key;
            msg += ": \"";
            msg += std::string{raw_value.text};
            msg += "\"";
            return fail(std::move(msg));
        }

        if (auto* ptr = std::get_if<std::int64_t*>(&opt.target))
        {
            assert(*ptr != nullptr);
            **ptr = parsed_value;
            return true;
        }

        if (auto* ptr = std::get_if<std::int32_t*>(&opt.target))
        {
            assert(*ptr != nullptr);
            if (parsed_value < std::numeric_limits<std::int32_t>::min() ||
                parsed_value > std::numeric_limits<std::int32_t>::max())
            {
                std::string msg = "Integer out of range for ";
                msg += option_key;
                msg += ": \"";
                msg += std::string{raw_value.text};
                msg += "\"";
                return fail(std::move(msg));
            }
            **ptr = static_cast<std::int32_t>(parsed_value);
            return true;
        }

        if (auto* ptr = std::get_if<std::uint64_t*>(&opt.target))
        {
            assert(*ptr != nullptr);
            if (parsed_value < 0)
            {
                std::string msg = "Invalid integer for ";
                msg += option_key;
                msg += ": \"";
                msg += std::string{raw_value.text};
                msg += "\"";
                return fail(std::move(msg));
            }
            **ptr = static_cast<std::uint64_t>(parsed_value);
            return true;
        }

        if (auto* ptr = std::get_if<std::uint32_t*>(&opt.target))
        {
            assert(*ptr != nullptr);
            if (parsed_value < 0 ||
                parsed_value >
                        static_cast<std::int64_t>(
                                std::numeric_limits<std::uint32_t>::max()))
            {
                std::string msg = "Integer out of range for ";
                msg += option_key;
                msg += ": \"";
                msg += std::string{raw_value.text};
                msg += "\"";
                return fail(std::move(msg));
            }
            **ptr = static_cast<std::uint32_t>(parsed_value);
            return true;
        }

        if (auto* ptr = std::get_if<std::uint16_t*>(&opt.target))
        {
            assert(*ptr != nullptr);
            if (parsed_value < 0 ||
                parsed_value >
                        static_cast<std::int64_t>(
                                std::numeric_limits<std::uint16_t>::max()))
            {
                std::string msg = "Integer out of range for ";
                msg += option_key;
                msg += ": \"";
                msg += std::string{raw_value.text};
                msg += "\"";
                return fail(std::move(msg));
            }
            **ptr = static_cast<std::uint16_t>(parsed_value);
            return true;
        }

        if (auto* ptr = std::get_if<std::int16_t*>(&opt.target))
        {
            assert(*ptr != nullptr);
            if (parsed_value <
                        static_cast<std::int64_t>(
                                std::numeric_limits<std::int16_t>::min()) ||
                parsed_value >
                        static_cast<std::int64_t>(
                                std::numeric_limits<std::int16_t>::max()))
            {
                std::string msg = "Integer out of range for ";
                msg += option_key;
                msg += ": \"";
                msg += std::string{raw_value.text};
                msg += "\"";
                return fail(std::move(msg));
            }
            **ptr = static_cast<std::int16_t>(parsed_value);
            return true;
        }

        if (auto* ptr = std::get_if<std::uint8_t*>(&opt.target))
        {
            assert(*ptr != nullptr);
            if (parsed_value < 0 ||
                parsed_value >
                        static_cast<std::int64_t>(
                                std::numeric_limits<std::uint8_t>::max()))
            {
                std::string msg = "Integer out of range for ";
                msg += option_key;
                msg += ": \"";
                msg += std::string{raw_value.text};
                msg += "\"";
                return fail(std::move(msg));
            }
            **ptr = static_cast<std::uint8_t>(parsed_value);
            return true;
        }

        if (auto* ptr = std::get_if<std::int8_t*>(&opt.target))
        {
            assert(*ptr != nullptr);
            if (parsed_value <
                        static_cast<std::int64_t>(
                                std::numeric_limits<std::int8_t>::min()) ||
                parsed_value > static_cast<std::int64_t>(
                                       std::numeric_limits<std::int8_t>::max()))
            {
                std::string msg = "Integer out of range for ";
                msg += option_key;
                msg += ": \"";
                msg += std::string{raw_value.text};
                msg += "\"";
                return fail(std::move(msg));
            }
            **ptr = static_cast<std::int8_t>(parsed_value);
            return true;
        }

        result = parse_result::failure(
                parse_error::invalid_value,
                std::string{"unsupported target type for option: "} +
                        std::string{option_key});
        return false;
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