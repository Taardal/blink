#pragma once

#include "Environment.h"
#include <string>

#if defined(BL_PLATFORM_MACOS) || defined(BL_PLATFORM_LINUX)
    #define BL_PRINT_UNIX_STACKTRACE
#elif defined(BL_PLATFORM_WINDOWS)
    #define BL_PRINT_WINDOWS_STACKTRACE
#endif

namespace Blink {

    void addSignalHandlers();

    void onSignal(int signal);

    std::string getSignalName(int signal);

#ifdef BL_PRINT_UNIX_STACKTRACE
    void printUnixStacktrace();

    void demangleUnixSymbol(std::string& symbol);
#endif
}