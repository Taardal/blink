#include "Log.h"
#include <sstream>

namespace Blink {
    void Log::setLevel(LogLevel level) {
        spdlog::set_level(getSpdLogLevel(level));
    }

    std::string Log::formatMessage(
        const char* filename,
        const char* functionName,
        uint32_t lineNumber,
        std::string_view message
    ) {
        std::stringstream ss;
        ss << "[" << filename << ":" << functionName << ":" << lineNumber << "] - " << message;
        return ss.str();
    }

    spdlog::level::level_enum Log::getSpdLogLevel(LogLevel level) {
        switch (level) {
            case LogLevel::Critical:
                return spdlog::level::critical;
            case LogLevel::Error:
                return spdlog::level::err;
            case LogLevel::Warn:
                return spdlog::level::warn;
            case LogLevel::Info:
                return spdlog::level::info;
            case LogLevel::Debug:
                return spdlog::level::debug;
            default:
                return spdlog::level::trace;
        }
    }
}
