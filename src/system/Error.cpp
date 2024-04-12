#include "Error.h"
#include <iomanip>

namespace Blink {
    Error::Error(const std::string& message, const std::string& tag, const std::shared_ptr<Error>& previousError)
        : message(message), tag(tag), previousError(previousError) {
    }

    const char* Error::what() const noexcept {
        return message.c_str();
    }

    std::vector<StacktraceEntry> Error::getStacktrace() const {
        std::vector<StacktraceEntry> stack;
        auto currentError = std::make_shared<Error>(*this);
        while (currentError != nullptr) {
            StacktraceEntry entry{};
            entry.message = currentError->message;
            entry.tag = currentError->tag;
            stack.emplace(stack.begin(), entry);
            currentError = currentError->previousError;
        }
        return stack;
    }

    void Error::printStacktrace() const {
        fprintf(stderr, "Stacktrace (%s):\n", BL_TO_STRING(Blink::Error));
        std::vector<StacktraceEntry> stack = getStacktrace();
        uint32_t longestTagLength = 0;
        for (uint32_t i = 0; i < stack.size(); i++) {
            StacktraceEntry& stackEntry = stack[i];
            uint32_t tagLength = stackEntry.tag.size();
            if (tagLength > longestTagLength) {
                longestTagLength = tagLength;
            }
        }
        static constexpr uint8_t segmentSpacing = 4;
        for (uint32_t i = 0; i < stack.size(); i++) {
            StacktraceEntry& stackEntry = stack[i];
            uint32_t tagLength = stackEntry.tag.size();
            uint32_t tagRightSpacing = segmentSpacing + tagLength + (longestTagLength - tagLength);
            fprintf(stderr, "%*d", -segmentSpacing, i);
            fprintf(stderr, "%*s", -tagRightSpacing, stackEntry.tag.c_str());
            fprintf(stderr, "%s", stackEntry.message.c_str());
            fprintf(stderr, "\n");
        }
    }

    std::ostream& operator<<(std::ostream& os, const Error& error) {
        os << "Stacktrace (" << BL_TO_STRING(Blink::Error) << "):\n";
        std::vector<StacktraceEntry> stack = error.getStacktrace();
        uint32_t longestTagLength = 0;
        for (const auto& stackEntry : stack) {
            uint32_t tagLength = stackEntry.tag.size();
            longestTagLength = std::max(longestTagLength, tagLength);
        }
        static constexpr uint8_t segmentSpacing = 4;
        for (size_t i = 0; i < stack.size(); i++) {
            const auto& stackEntry = stack[i];
            os << std::setw(segmentSpacing) << std::left << i;
            os << std::setw(segmentSpacing + longestTagLength) << std::left << stackEntry.tag;
            os << stackEntry.message;
            os << "\n";
        }
        return os;
    }
}
