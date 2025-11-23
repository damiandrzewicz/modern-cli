#ifndef MCLI_DETAIL_CONFIG_HPP
#define MCLI_DETAIL_CONFIG_HPP

#if defined(_MSC_VER)
#define MCLI_COMPILER_MSVC 1
#elif defined(__clang__)
#define MCLI_COMPILER_CLANG 1
#elif defined(__GNUC__)
#define MCLI_COMPILER_GCC 1
#endif

#define MCLI_UNUSED(x) (void) (x)

#endif  // MCLI_DETAIL_CONFIG_HPP
