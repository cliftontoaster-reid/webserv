#ifndef MON_NET_INCLUDE_DETECT_HPP
#define MON_NET_INCLUDE_DETECT_HPP

#if defined(_WIN32) || defined(_WIN64)  // Windows (32-bit and 64-bit)
#define OS_WINDOWS

#elif defined(__linux__)  // Linux
#define OS_LINUX

#elif defined(__APPLE__) && defined(__MACH__)  // macOS (Apple ecosystem)
#define OS_MACOS

#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || \
    defined(__DragonFly__)  // BSD variants
#define OS_BSD

#else  // Fallback for unknown/unsupported OS
#define OS_UNKNOWN
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_64
#elif defined(__i386) || defined(_M_IX86)
#define ARCH_32
#endif

#if defined(_MSC_VER)
#define COMPILER_MSVC
#elif defined(__GNUC__)
#define COMPILER_GCC
#elif defined(__clang__)
#define COMPILER_CLANG
#endif

#if defined(OS_WINDOWS)
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

/* ---- endianness detection ---- */

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

#endif