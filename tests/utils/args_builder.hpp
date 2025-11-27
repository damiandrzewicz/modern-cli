#ifndef ARGS_BUILDER_HPP_
#define ARGS_BUILDER_HPP_

#include <string>
#include <tuple>
#include <vector>

namespace test::utils
{

/**
 * @brief Build argc/argv from a vector of strings.
 *
 * The first element should be the program name (like argv[0]).
 * The returned argv is null-terminated (argv[argc] == nullptr).
 *
 * NOTE: This helper owns its internal storage in a static variable so
 * the returned argv pointers remain valid until the next call.
 * It's intended for simple, single-threaded test usage.
 */
inline std::tuple<int, char**> make_argv(std::vector<std::string> args)
{
    // Static storage to keep strings and pointer array alive.
    // Each call overwrites the previous contents.
    static std::vector<std::string> storage;
    static std::vector<char*> pointers;

    storage = std::move(args);

    pointers.clear();
    pointers.reserve(storage.size() + 1);

    for (auto& argument : storage)
    {
        pointers.push_back(argument.data());
    }

    // POSIX: argv[argc] is a null pointer.
    pointers.push_back(nullptr);

    const int argc = static_cast<int>(storage.size());
    char** argv = pointers.data();

    return {argc, argv};
}

}  // namespace test::utils

#endif  // ARGS_BUILDER_HPP_