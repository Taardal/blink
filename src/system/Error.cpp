#include "Error.h"

namespace Blink {
    Error::Error(const std::string& message, std::shared_ptr<Error> previousError)
            : errorMessage(message), previousError(previousError) {}

    const char* Error::what() const noexcept {
        return errorMessage.c_str();
    }

    std::vector<std::string> Error::getStack() const {
        std::vector<std::string> stack;
        auto currentError = std::make_shared<Error>(*this);
        while (currentError != nullptr) {
            stack.emplace(stack.begin(), currentError->what());
            currentError = currentError->previousError;
        }
        return stack;
    }

    void Error::printStack(LogLevel logLevel) const {
        Log::log(logLevel, "=============== ERROR STACK ===============");
        for (const std::string& errorMessage : getStack()) {
            Log::log(logLevel, "-- {}", errorMessage);
        }
        Log::log(logLevel, "===========================================");
    }
};
