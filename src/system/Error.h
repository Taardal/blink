#pragma once

#include "Log.h"
#include "ErrorSignal.h"

#include <iostream>

#define BL_THROW(message) throw Error(message, BL_TAG())

#define BL_ASSERT_THROW(expression) \
    if (expression) {\
        /* Continue */\
    } else {\
        throw Error(std::string("Assertion failed: ") + #expression, BL_TAG());\
    }

#define BL_EXECUTE_THROW(expression) \
    try {\
        expression;\
    } catch (const Error& e) { \
        throw Error(#expression, BL_TAG(), std::make_shared<Error>(e));\
    } catch (const std::exception& e) {\
        throw Error(#expression, BL_TAG(), std::make_shared<Error>(e.what()));\
    }

namespace Blink {
    struct StacktraceEntry {
        std::string message;
        std::string tag;
    };

    class Error final : public std::exception {
    private:
        std::string message;
        std::string tag;
        std::shared_ptr<Error> previousError = nullptr;

    public:
        Error(const std::string& message, const std::string& tag = "", const std::shared_ptr<Error>& previousError = nullptr);

        const char* what() const noexcept override;

        std::vector<StacktraceEntry> getStacktrace() const;

        void printStacktrace() const;

        static void printStacktrace(const Error& error);

        static void onSignal(const ::std::function<void(const ErrorSignal&)>& handler);
    };

    std::ostream& operator<<(std::ostream& os, const Error& error);
}