#pragma once

#include <spdlog/spdlog.h>
#include <string>

#ifndef __FILE_NAME__
    #define __FILE_NAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

#define BL_FILENAME __FILE_NAME__
#define BL_FUNCTION_NAME __func__
#define BL_LINE_NUMBER __LINE__

#define BL_TAG(message) ::Blink::Log::formatMessage(BL_FILENAME, BL_FUNCTION_NAME, BL_LINE_NUMBER, message)

#define BL_LOG_TRACE(message, ...) ::Blink::Log::trace(BL_TAG(message), ##__VA_ARGS__)
#define BL_LOG_DEBUG(message, ...) ::Blink::Log::debug(BL_TAG(message), ##__VA_ARGS__)
#define BL_LOG_INFO(message, ...) ::Blink::Log::info(BL_TAG(message), ##__VA_ARGS__)
#define BL_LOG_WARN(message, ...) ::Blink::Log::warn(BL_TAG(message), ##__VA_ARGS__)
#define BL_LOG_ERROR(message, ...) ::Blink::Log::error(BL_TAG(message), ##__VA_ARGS__)
#define BL_LOG_CRITICAL(message, ...) ::Blink::Log::critical(BL_TAG(message), ##__VA_ARGS__)

namespace Blink {
    enum class LogLevel {
        None = 0,
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Critical
    };

    class Log {
    public:
        template<typename... T>
        static void trace(std::string_view message, const T&... args) {
            spdlog::trace(message, args...);
        }

        template<typename... T>
        static void debug(std::string_view message, const T&... args) {
            spdlog::debug(message, args...);
        }

        template<typename... T>
        static void info(std::string_view message, const T&... args) {
            spdlog::info(message, args...);
        }

        template<typename... T>
        static void warn(std::string_view message, const T&... args) {
            spdlog::warn(message, args...);
        }

        template<typename... T>
        static void error(std::string_view message, const T&... args) {
            spdlog::error(message, args...);
        }

        template<typename... T>
        static void critical(std::string_view message, const T&... args) {
            spdlog::critical(message, args...);
        }

        template<typename... T>
        static void log(LogLevel logLevel, std::string_view message, const T&... args) {
            if (logLevel == LogLevel::Critical) {
                critical(message, args...);
            } else if (logLevel == LogLevel::Error) {
                error(message, args...);
            } else if (logLevel == LogLevel::Warn) {
                warn(message, args...);
            } else if (logLevel == LogLevel::Info) {
                info(message, args...);
            } else if (logLevel == LogLevel::Debug) {
                debug(message, args...);
            } else if (logLevel == LogLevel::Trace) {
                trace(message, args...);
            }
        }

        static void setLevel(LogLevel level);

        static std::string formatMessage(
            const char* filename,
            const char* functionName,
            uint32_t lineNumber,
            std::string_view message
        );

    private:
        static spdlog::level::level_enum getSpdLogLevel(LogLevel level);
    };
}
