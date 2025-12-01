#ifndef MCLI_DETAIL_PARSE_PARSE_RESULT_HPP_
#define MCLI_DETAIL_PARSE_PARSE_RESULT_HPP_

#include <string>

namespace mcli::detail::parse
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

}  // namespace mcli::detail::parse

#endif  // MCLI_DETAIL_PARSE_PARSE_RESULT_HPP_