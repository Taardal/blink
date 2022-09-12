#pragma once

#ifdef NDEBUG
    #define BL_RELEASE
#else
    #define BL_DEBUG
#endif

#ifdef BL_DEBUG
    #include "Log.h"
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
    #define BL_ASSERT(tag, expression) \
        if (expression) \
        {} \
        else \
        { \
            BL_LOG_ERROR(tag, "Could not assert [{0}]", BL_TO_STRING(expression)); \
            BL_BREAK(); \
        }
#else
    #define BL_BREAK()
    #define BL_ASSERT(tag, expression)
#endif