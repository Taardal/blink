#pragma once

#include "system/Environment.h"

#ifdef BL_DEBUG
    #define BL_ENABLE_BREAK
    #define BL_ENABLE_ASSERT
#endif

#ifdef BL_ENABLE_BREAK
    #ifdef BL_PLATFORM_WINDOWS
        #define BL_BREAK() __debugbreak()
    #elif __has_builtin(__builtin_debugtrap)
        #define BL_BREAK() __builtin_debugtrap()
    #else
        #include <csignal>
        #if defined(SIGTRAP)
            #define BL_BREAK() std::raise(SIGTRAP)
        #else
            #define BL_BREAK() std::raise(SIGABRT)
        #endif
    #endif
#else
    #define BL_BREAK()
#endif

#ifdef BL_ENABLE_ASSERT
    #define BL_ASSERT(expression) \
        if (expression) \
        {} \
        else \
        { \
            BL_LOG_CRITICAL("Could not assert [{0}]", #expression); \
            BL_BREAK(); \
        }
#else
    #define BL_ASSERT(expression)
#endif