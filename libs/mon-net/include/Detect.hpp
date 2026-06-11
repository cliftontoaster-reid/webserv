#ifndef MON_NET_INCLUDE_DETECT_HTTP
#define MON_NET_INCLUDE_DETECT_HTTP

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

#endif