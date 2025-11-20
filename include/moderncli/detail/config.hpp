#ifndef MODERNCLI_DETAIL_CONFIG_HPP
#define MODERNCLI_DETAIL_CONFIG_HPP

#if defined(_MSC_VER)
#  define MODERNCLI_COMPILER_MSVC 1
#elif defined(__clang__)
#  define MODERNCLI_COMPILER_CLANG 1
#elif defined(__GNUC__)
#  define MODERNCLI_COMPILER_GCC 1
#endif

#define MODERNCLI_UNUSED(x) (void)(x)

#endif // MODERNCLI_DETAIL_CONFIG_HPP
