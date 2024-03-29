#pragma once

#include <spdlog/spdlog.h>
#include <string>

#ifndef __FILE_NAME__
    #define __FILE_NAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

#define BL_FILENAME __FILE_NAME__
#define BL_FUNCTION_NAME __func__
#define BL_LINE_NUMBER __LINE__

#define BL_TAG(message) ::Blink::Log::Format(BL_FILENAME, BL_FUNCTION_NAME, BL_LINE_NUMBER, message)

#define BL_LOG_TRACE(message, ...) ::Blink::Log::Trace(BL_TAG(message), ##__VA_ARGS__)
#define BL_LOG_DEBUG(message, ...) ::Blink::Log::Debug(BL_TAG(message), ##__VA_ARGS__)
#define BL_LOG_INFO(message, ...) ::Blink::Log::Info(BL_TAG(message), ##__VA_ARGS__)
#define BL_LOG_WARN(message, ...) ::Blink::Log::Warn(BL_TAG(message), ##__VA_ARGS__)
#define BL_LOG_ERROR(message, ...) ::Blink::Log::Error(BL_TAG(message), ##__VA_ARGS__)
#define BL_LOG_CRITICAL(message, ...) ::Blink::Log::Critical(BL_TAG(message), ##__VA_ARGS__)

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
        static void SetLevel(LogLevel level);

        template<typename... T>
        static void Trace(std::string_view message, const T&... args) {
            spdlog::trace(message, args...);
        }

        template<typename... T>
        static void Debug(std::string_view message, const T&... args) {
            spdlog::debug(message, args...);
        }

        template<typename... T>
        static void Info(std::string_view message, const T&... args) {
            spdlog::info(message, args...);
        }

        template<typename... T>
        static void Warn(std::string_view message, const T&... args) {
            spdlog::warn(message, args...);
        }

        template<typename... T>
        static void Error(std::string_view message, const T&... args) {
            spdlog::error(message, args...);
        }

        template<typename... T>
        static void Critical(std::string_view message, const T&... args) {
            spdlog::critical(message, args...);
        }

        static std::string Format(
            const char* filename,
            const char* functionName,
            uint32_t lineNumber,
            std::string_view message
        );

    private:
        static spdlog::level::level_enum GetSpdLogLevel(LogLevel level);
    };
}
