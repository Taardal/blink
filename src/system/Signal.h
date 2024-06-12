#pragma once

#include "Environment.h"
#include <string>

#if defined(BL_PLATFORM_MACOS) || defined(BL_PLATFORM_LINUX)
    #define BL_PRINT_UNIX_STACKTRACE
#elif defined(BL_PLATFORM_WINDOWS)
    #define BL_PRINT_WINDOWS_STACKTRACE
#endif

namespace Blink {
    struct ErrorSignal {
        int code;
        std::string name;
        std::string description;

        ErrorSignal(int signal);

        void printStacktrace() const;

    // STATIC
    private:
        static std::function<void(const ErrorSignal&)> handlerFn;

    public:
        static void setHandler(const std::function<void(const ErrorSignal&)>& handler);

    private:
        static void handleErrorSignal(int signal);

        static void printStacktrace(const ErrorSignal& signal);

#ifdef BL_PRINT_UNIX_STACKTRACE
        static void printUnixStacktrace();

        static void demangleUnixStacktraceLine(std::string* stacktraceLine);
#endif
    };
}