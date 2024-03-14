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

#if defined(NDEBUG)
    #define BL_RELEASE
    #define BL_BUILD_RELEASE
#else
    #define BL_DEBUG
    #define BL_BUILD_DEBUG
#endif

namespace Blink {

    enum class Platform {
        None = 0,
        Windows = 1,
        MacOS = 2,
        Linux = 3,
    };

    enum class Compiler {
        None = 0,
        MSVC = 1,
        Clang = 2,
        GCC = 3,
    };

    enum class Build {
        None = 0,
        Debug = 1,
        Release = 2,
    };

    class Environment {
    public:
        static Platform getPlatform() {
#if defined(BL_PLATFORM_WINDOWS)
            return Platform::Windows;
#elif defined(BL_PLATFORM_MACOS)
            return Platform::MacOS;
#elif defined(BL_PLATFORM_LINUX)
            return Platform::Linux;
#endif
        }

        static bool isWindows() {
            return getPlatform() == Platform::Windows;
        }

        static bool isMacOS() {
            return getPlatform() == Platform::MacOS;
        }

        static bool isLinux() {
            return getPlatform() == Platform::Linux;
        }

        static Compiler getCompiler() {
#if defined(BL_COMPILER_MSVC)
            return Compiler::MSVC;
#elif defined(BL_COMPILER_CLANG)
            return Compiler::Clang;
#elif defined(BL_COMPILER_GCC)
            return Compiler::GCC;
#endif
        }

        static bool isMSVC() {
            return getCompiler() == Compiler::MSVC;
        }

        static bool isClang() {
            return getCompiler() == Compiler::Clang;
        }

        static bool isGCC() {
            return getCompiler() == Compiler::GCC;
        }

        static Build getBuild() {
#if defined(BL_BUILD_RELEASE)
            return Build::Release;
#elif defined(BL_BUILD_DEBUG)
            return Build::Debug;
#endif
        }

        static bool isRelease() {
            return getBuild() == Build::Release;
        }

        static bool isDebug() {
            return getBuild() == Build::Debug;
        }
    };
}