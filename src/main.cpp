#include "pch.h"
#include "AppConfig.h"
#include "App.h"

#include <cxxabi.h>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

using namespace Blink;

std::string getSignalName(const int signal) {
    switch (signal) {
        case SIGSEGV:
            return "SIGSEGV";
        case SIGABRT:
            return "SIGABRT";
        case SIGILL:
            return "SIGILL";
        case SIGFPE:
            return "SIGFPE";
        case SIGBUS:
            return "SIGBUS";
        default:
            return "";
    }
}

// Example: 1   libsystem_platform.dylib            0x0000000189879a24 _sigtramp + 56
// The symbolic representation of each address consists of
//  - The function name (if this can be determined)
//  - A hexadecimal offset into the function (number of bytes into the function (Number of characters ≈ Hexadecimal offset / Average bytes per character)
//  - The actual return address (in hexadecimal)

// 0: The demangling operation succeeded.
// -1: A memory allocation failiure occurred.
// -2: mangled_name is not a valid name under the C++ ABI mangling rules.
// -3: One of the arguments is invalid.

void onSignal(int signal) {
    int maxStackSize = 10;
    void* stack[maxStackSize];

    std::string signalName = getSignalName(signal);

    int stackSize = backtrace(stack, maxStackSize);
    //backtrace_symbols_fd(stack, stackSize, STDERR_FILENO);

    fprintf(stderr, "Error: signal [%s]:\n", signalName.c_str());

    char** symbols = backtrace_symbols(stack, stackSize);
    if (symbols == nullptr) {
        fprintf(stderr, "Could not resolve stacktrace\n");
        return;
    }

    fprintf(stderr, "Stacktrace (raw):\n");
    for (int i = 0; i < stackSize; i++) {
        char* symbol = symbols[i];
        fprintf(stderr, "%s\n", symbol);
    }

    fprintf(stderr, "Stacktrace (demangled):\n");
    for (int i = 0; i < stackSize; i++) {
        char* symbol = symbols[i];
        std::string symbolString(symbol);

        size_t returnAddressStartIndex = symbolString.find("0x");
        size_t returnAddressEndIndex = returnAddressStartIndex + 18; // 18 = return address length

        size_t functionNameStartIndex = returnAddressEndIndex + 1; // +1 = space
        size_t functionNameEndIndex = symbolString.find(" + ");

        auto functionName = symbolString.substr(functionNameStartIndex, functionNameEndIndex - functionNameStartIndex);

        int status;
        char* outputBuffer = nullptr;
        size_t* length = nullptr;
        char* demangledFunctionName = abi::__cxa_demangle(functionName.c_str(), outputBuffer, length, &status);

        bool successfullyDemangled = status == 0 && demangledFunctionName != nullptr;
        fprintf(stderr, "%d\t%s\n", i, successfullyDemangled ? demangledFunctionName : functionName.c_str());

        free(demangledFunctionName);
    }

    free(symbols);
    exit(1);
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        // Skip empty tokens (e.g., consecutive delimiters)
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

int main() {
    Log::setLevel(LogLevel::Debug);

    // (Segmentation fault): Indicates invalid memory access.
    signal(SIGSEGV, onSignal);
    // (Abort): Indicates an abnormal termination initiated by the program itself.
    signal(SIGABRT, onSignal);
    // (Illegal instruction): Indicates an attempt to execute an illegal instruction.
    signal(SIGILL, onSignal);
    // (Floating-point exception): Indicates arithmetic exceptions like division by zero or floating-point overflow.
    signal(SIGFPE, onSignal);
    // (Bus error): Indicates an attempt to access memory that the CPU cannot physically address.
    signal(SIGBUS, onSignal);

    std::string input = "1   libsystem_platform.dylib            0x0000000189879a24 _sigtramp + 56";

    std::vector<std::string> parts = splitString(input, ' ');

    // Output the result
    for (const auto& part : parts) {
        std::cout << part << std::endl;
    }

return 0;

    AppConfig config;
    config.name = "Blink";
    config.windowWidth = 1280;
    config.windowHeight = 768;
    config.windowResizable = true;

    auto app = new App(config);
    app->run();
    delete app;

    return EXIT_SUCCESS;
}
