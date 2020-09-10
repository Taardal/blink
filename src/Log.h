#pragma once

#include "Tag.h"
#include <spdlog/spdlog.h>
#include <string>

#define ST_LOG_TRACE(tag, message, ...) ::blink::Log::Trace(::blink::Log::Format(tag, message), ##__VA_ARGS__)
#define ST_LOG_DEBUG(tag, message, ...) ::blink::Log::Debug(::blink::Log::Format(tag, message), ##__VA_ARGS__)
#define ST_LOG_INFO(tag, message, ...) ::blink::Log::Info(::blink::Log::Format(tag, message), ##__VA_ARGS__)
#define ST_LOG_WARN(tag, message, ...) ::blink::Log::Warn(::blink::Log::Format(tag, message), ##__VA_ARGS__)
#define ST_LOG_ERROR(tag, message, ...) ::blink::Log::Error(::blink::Log::Format(tag, message), ##__VA_ARGS__)
#define ST_LOG_CRITICAL(tag, message, ...) ::blink::Log::Critical(::blink::Log::Format(tag, message), ##__VA_ARGS__)

namespace blink
{
    enum class LogLevel
    {
        None = 0,
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Critical
    };

    class Log
    {
    public:
        static void SetLevel(LogLevel level);

        template<typename... T>
        static void Trace(std::string_view message, const T& ... args)
        {
            spdlog::trace(message, args...);
        }

        template<typename... T>
        static void Debug(std::string_view message, const T& ... args)
        {
            spdlog::debug(message, args...);
        }

        template<typename... T>
        static void Info(std::string_view message, const T& ... args)
        {
            spdlog::info(message, args...);
        }

        template<typename... T>
        static void Warn(std::string_view message, const T& ... args)
        {
            spdlog::warn(message, args...);
        }

        template<typename... T>
        static void Error(std::string_view message, const T& ... args)
        {
            spdlog::error(message, args...);
        }

        template<typename... T>
        static void Critical(std::string_view message, const T& ... args)
        {
            spdlog::critical(message, args...);
        }

        static std::string Format(std::string_view tag, std::string_view message);

    private:
        static spdlog::level::level_enum GetSpdLogLevel(LogLevel level);
    };

}