#pragma once

#include "system/Log.h"
#include "system/Environment.h"
#include "system/Error.h"

#include <iostream>

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

#define BL_THROW(error) throw Error(BL_TAG(error))

#define BL_ASSERT_THROW(expression) \
    if (expression) {\
        /* Continue */\
    } else {\
        BL_THROW(std::string("Assertion failed: ") + #expression);\
    }

#define BL_TRY(expression) \
    try {\
        expression;\
    } catch (const Error& e) { \
        throw Error(BL_TAG(#expression), std::make_shared<Error>(e));\
    } catch (const std::exception& e) {\
        throw Error(BL_TAG(#expression), std::make_shared<Error>(e.what()));\
    }
