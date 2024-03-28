#pragma once

#include "system/Log.h"
#include "system/Environment.h"

#ifdef BL_DEBUG
    #define BL_ENABLE_BREAK
    #define BL_ENABLE_ASSERT
#endif

#ifdef BL_ENABLE_BREAK
    #if defined(BL_COMPILER_MSVC)
        #define BL_BREAK() __debugbreak()
    #elif defined(BL_COMPILER_CLANG) || defined(BL_COMPILER_GCC)
        #define BL_BREAK() __builtin_trap()
    #else
        #error "BL_BREAK() not implemented for this compiler"
    #endif
#else
    #define BL_BREAK()
#endif

#ifdef BL_ENABLE_ASSERT
    #define BL_ASSERT(expression) \
        if (expression) {\
            /* Continue */\
        } else {\
            BL_LOG_CRITICAL("Assertion failed: {}", #expression); \
            BL_BREAK(); \
        }
#else
    #define BL_ASSERT(expression)
#endif

#define BL_ASSERT_THROW(expression) \
    if (expression) {\
        /* Continue */\
    } else {\
        throw std::runtime_error(BL_TAG(std::string("Assertion failed: ") + #expression));\
    }