#pragma once

#include "Environment.h"
#include <string>

#if defined(BL_PLATFORM_MACOS) || defined(BL_PLATFORM_LINUX)
    #define BL_PRINT_UNIX_STACKTRACE
#elif defined(BL_PLATFORM_WINDOWS)
    #define BL_PRINT_WINDOWS_STACKTRACE
#endif

namespace Blink {

    struct Signal {
        int code;
        std::string name;
        std::string description;

        static Signal create(int signal);

        void printStacktrace() const;

        static void printStacktrace(const Signal& signal);
    };

    class SignalHandler {
    private:
        static std::map<int, std::vector<std::function<void(int)>>> handlers;
        static std::function<void(const Signal&)> errorHandler;

    public:
        static void setErrorSignalHandler(const std::function<void(const Signal&)>& handler);

        static void onErrorSignal(int signal);

        static void initialize();

        static void addHandler(int signal, const std::function<void(int)>& handler);

    private:
        static void onSignal(int signal);

        static bool shouldExit(int signal);

        static bool shouldPrintStacktrace(int signal);
    };

    void addErrorSignalStacktracePrinters();

    void printStacktrace(int signal);

    std::string getSignalName(int signal);

#ifdef BL_PRINT_UNIX_STACKTRACE
    void printUnixStacktrace();

    void demangleUnixStacktraceLine(std::string* stacktraceLine);
#endif
}