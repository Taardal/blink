#pragma once

#include "Log.h"
#include <iostream>

namespace Blink {
    class Error : public std::exception {
    private:
        std::string errorMessage;
        std::shared_ptr<Error> previousError = nullptr;

    public:
        Error(const std::string& message, std::shared_ptr<Error> previousError = nullptr);

        const char* what() const noexcept override;

        std::vector<std::string> getStack() const;

        void printStack(::Blink::LogLevel logLevel = LogLevel::Error) const;
    };

}