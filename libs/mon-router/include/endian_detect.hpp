/* endian_detect.hpp — compile‑time endianness detection
   Works with GCC, Clang, ICC, ARMCC, SunCC, and most Unix‑like headers.
*/

#ifndef ENDIAN_DETECT_HPP
#define ENDIAN_DETECT_HPP

/* 1. GCC / Clang (and many other GCC‑compatible front‑ends) */
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && \
    defined(__ORDER_BIG_ENDIAN__)

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define HOST_IS_LITTLE_ENDIAN 1
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define HOST_IS_BIG_ENDIAN 1
#else
#error "Unknown byte order reported by __BYTE_ORDER__"
#endif

/* 2. ARMCC / Keil (defines __BIG_ENDIAN or __LITTLE_ENDIAN as 1) */
#elif defined(__BIG_ENDIAN) && (__BIG_ENDIAN == 1)
#define HOST_IS_BIG_ENDIAN 1
#elif defined(__LITTLE_ENDIAN) && (__LITTLE_ENDIAN == 1)
#define HOST_IS_LITTLE_ENDIAN 1

/* 3. Sun / Solaris */
#elif defined(_BIG_ENDIAN)
#define HOST_IS_BIG_ENDIAN 1
#elif defined(_LITTLE_ENDIAN)
#define HOST_IS_LITTLE_ENDIAN 1

/* 4. Fallback to <endian.h> if we are on a POSIX system */
#elif defined(__unix__) || defined(__APPLE__) || defined(__MACH__)
#include <endian.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define HOST_IS_LITTLE_ENDIAN 1
#elif __BYTE_ORDER == __BIG_ENDIAN
#define HOST_IS_BIG_ENDIAN 1
#else
#error "Unsupported byte order in <endian.h>"
#endif

/* 5. MSVC – no compile‑time macro, use a runtime check that can be turned
   into a const expression by the optimizer. */
#elif defined(_MSC_VER)
/* MSVC runs on little‑endian x86/x64, so we can safely assume it. */
#define HOST_IS_LITTLE_ENDIAN 1
#else
#error "Unable to detect endianness for this compiler/platform"
#endif

/* Optional helper macro for readability */
#if defined(HOST_IS_LITTLE_ENDIAN)
#define IS_LITTLE_ENDIAN 1
#else
#define IS_LITTLE_ENDIAN 0
#endif

#endif /* ENDIAN_DETECT_HPP */
