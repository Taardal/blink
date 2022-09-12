#pragma once

#if defined(_WIN32)
    #if defined(_WIN64)
        #define BL_PLATFORM_WINDOWS
    #else
        #error "Unsupported platform: Windows x86"
    #endif
#elif defined(__APPLE__) || defined(__MACH__)
    #define BL_PLATFORM_MACOS
#else
    #error "Unknown platform"
#endif

#if defined(_MSC_VER)
    #define BL_COMPILER_MSVC
#elif defined(__clang__)
    #define BL_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define BL_COMPILER_GCC
#else
    #error "Unknown compiler"
#endif