#ifndef MCLI_CORE_HPP_
#define MCLI_CORE_HPP_

#include <cassert>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace mcli
{

enum class parse_error
{
    none,
    unknown_option,
    duplicate_option,
    invalid_value,
};

class parse_result
{
public:
    parse_result() = default;

    static parse_result success()
    {
        return parse_result{};
    }

    static parse_result failure(parse_error error_code, std::string msg)
    {
        parse_result res;
        res.m_error = error_code;
        res.m_message = std::move(msg);
        return res;
    }

    explicit operator bool() const noexcept
    {
        return m_error == parse_error::none;
    }

    [[nodiscard]] parse_error error_code() const noexcept
    {
        return m_error;
    }

    [[nodiscard]] std::string_view error_message() const noexcept
    {
        return m_message;
    }

private:
    parse_error m_error{parse_error::none};
    std::string m_message;
};

enum class option_kind
{
    flag
};

enum class value_kind
{
    boolean
};

struct option_spec
{
    std::string name;         // "--verbose"
    std::string description;  // help text
    option_kind kind{option_kind::flag};
    std::variant<std::monostate, bool*> target{std::monostate{}};
    value_kind vkind{value_kind::boolean};
    bool seen{false};  // for duplicate detection
};

class command;  // forward declaration

/**
 * @brief Reference to an option variable.
 *
 * Used to bind command-line options to user variables.
 */
class option_ref
{
public:
    option_ref() = default;

private:
    friend class command;
    explicit option_ref(option_spec*) {}
};

/**
 * @@brief Core command class.
 */
class command
{
public:
    command() = default;

    // ---- Binding APIs ----
    struct FlagSpec
    {
        std::string_view name;
        std::string_view description;
        std::reference_wrapper<bool> target;
    };

    // Bind a boolean flag to a bool reference (e.g. opts.verbose).
    option_ref flag(const FlagSpec& fspec)
    {
        auto option_name = std::string{fspec.name};
        // ensure_unique_name(option_name);

        option_spec spec;
        spec.name = std::move(option_name);
        spec.description.assign(fspec.description.begin(),
                                fspec.description.end());
        spec.vkind = value_kind::boolean;
        spec.target = &fspec.target.get();
        spec.seen = false;

        m_options.push_back(std::move(spec));
        auto* added = &m_options.back();
        m_index_by_name.emplace(added->name, m_options.size() - 1);

        return option_ref{added};
    }

    // ---- Parsing ----
    [[nodiscard]] parse_result parse(int argc, char** argv)
    {
        // reset_seen_flags();
        parse_result result = parse_result::success();
        std::optional<std::string> active_subcommand;

        parse_range(*this, argc, argv, 1, result);

        // if (result && active_subcommand)
        // {
        //     result.set_active_subcommand(*active_subcommand);
        // }

        return result;
    }

private:
    static void parse_range(command& cmd,
                            int argc,
                            char** argv,
                            int start_index,
                            parse_result& result)
    {
        std::span<char*> args(argv, static_cast<std::size_t>(argc));
        for (char* arg : args.subspan(static_cast<std::size_t>(start_index)))
        {
            std::string_view tok{arg};
            if (!handle_token(cmd, tok, result))
            {
                return;
            }
        }
    }

    // Handle a single token; returns true to continue parsing, false to stop on
    // error.
    static bool handle_token(command& cmd,
                             std::string_view tok,
                             parse_result& result)
    {
        if (tok.empty())
        {
            return true;
        }

        // Option (flag)
        auto opt_index = find_option_index(cmd, tok);
        if (!opt_index.has_value())
        {
            result = parse_result::failure(
                    parse_error::unknown_option,
                    "Unknown option: " + std::string{tok});
            return false;
        }

        auto& opt = cmd.m_options[*opt_index];

        if (opt.seen)
        {
            result = parse_result::failure(parse_error::duplicate_option,
                                           "Duplicate option: " + opt.name);
            return false;
        }

        opt.seen = true;
        return apply_option(opt, result);
    }

    // Find option index by name.
    static std::optional<std::size_t> find_option_index(command& cmd,
                                                        std::string_view tok)
    {
        auto iter = cmd.m_index_by_name.find(std::string{tok});
        if (iter == cmd.m_index_by_name.end())
        {
            return std::nullopt;
        }
        return iter->second;
    }

    // Apply the option semantics; returns true to continue parsing, false to
    // stop on error.
    static bool apply_option(option_spec& opt, parse_result& /*result*/)
    {
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
    static void apply_flag(option_spec& opt)
    {
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

    std::vector<option_spec> m_options;
    std::unordered_map<std::string, std::size_t> m_index_by_name;
};

/**
 * @brief Command class representing a CLI command.
 *
 * Top-level helper to build a command using a DSL lambda.
 */
template <typename Def>
[[nodiscard]] command define(Def&& def)
{
    command cmd;
    def(cmd);
    return cmd;
}

}  // namespace mcli

#endif  // MCLI_CORE_HPP_