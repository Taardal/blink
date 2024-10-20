#pragma once

#include "Environment.h"
#include <string>

#if defined(BL_PLATFORM_MACOS) || defined(BL_PLATFORM_LINUX)
    #define BL_PRINT_UNIX_STACKTRACE
    #include <cxxabi.h>
    #include <execinfo.h>
#elif defined(BL_PLATFORM_WINDOWS)
    #define BL_PRINT_WINDOWS_STACKTRACE
#endif

namespace Blink {
    void initializeErrorSignalHandlers();

    void handleErrorSignal(int signal);

    void printStacktrace(int signal);

    std::string getSignalName(int signal);

    std::string getSignalDescription(int signal);

#ifdef BL_PRINT_UNIX_STACKTRACE
    void printUnixStacktrace();

    void demangleUnixStacktraceLine(std::string* stacktraceLine);
#endif
}
