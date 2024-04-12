#include "Signal.h"

namespace Blink {
    void addSignalHandlers() {
        // (Segmentation fault): Invalid memory access.
        signal(SIGSEGV, onSignal);
        // (Abort): Abnormal termination initiated by the program itself.
        signal(SIGABRT, onSignal);
        // (Illegal instruction): Attempt to execute an illegal instruction.
        signal(SIGILL, onSignal);
        // (Floating-point exception): Arithmetic exceptions like division by zero or floating-point overflow.
        signal(SIGFPE, onSignal);
        // (Bus error): Attempt to access memory that the CPU cannot physically address.
        signal(SIGBUS, onSignal);
    }

    void onSignal(int signal) {
        std::string signalName = getSignalName(signal);
        fprintf(stderr, "Signal [%s]:\n", signalName.c_str());
        fprintf(stderr, "Stacktrace:\n");
#if defined(BL_PRINT_UNIX_STACKTRACE)
        printUnixStacktrace();
#elif defined(BL_PRINT_WINDOWS_STACKTRACE)
        fprintf(stderr, "Could not print stacktrace for Windows, not implemented\n");
#else
        fprintf(stderr, "Could not print stacktrace, unsupported platform\n");
#endif
        exit(1);
    }

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

#ifdef BL_PRINT_UNIX_STACKTRACE

#include <cxxabi.h>
#include <execinfo.h>
#include <unistd.h>

    void printUnixStacktrace() {
        constexpr int maxStackSize = 15;
        void* stack[maxStackSize];
        int stackSize = backtrace(stack, maxStackSize);
        char** symbols = backtrace_symbols(stack, stackSize);
        if (symbols == nullptr) {
            fprintf(stderr, "Could not resolve stacktrace\n");
            return;
        }
        for (int i = 0; i < stackSize; i++) {
            ::std::string symbol(symbols[i]);
            demangleUnixSymbol(symbol);
            fprintf(stderr, "%s\n", symbol.c_str());
        }
        free(symbols);
    }

    // "Mangling" is a technique used by compilers to encode additional information into symbol names in compiled code.
    // This additional information includes things like function names, namespaces, argument types, and template parameters.
    // We want to "demangle" (i.e. decode) this information to make it human-readable.
    //
    // Mangled symbol:
    // ---
    // 10  blink  0x0000000102cbfe5c  _ZN5Blink3App10initializeEv + 1960
    //
    // Demangled symbol:
    // ---
    // 10  blink  0x00000001028b3e50  Blink::App::initialize() + 1960
    //
    // Segments:
    // ---
    // [stack index]
    // [binary name]
    // [return address (in hexadecimal)]
    // [function name + offset into the function (in hexadecimal)]
    void demangleUnixSymbol(::std::string& symbol) {

        // Find segment before the mangled function name
        size_t returnAddressStartIndex = symbol.find("0x");
        size_t returnAddressEndIndex = returnAddressStartIndex + 18; // 18 = return address length

        // Find function name start and end
        size_t functionNameStartIndex = returnAddressEndIndex + 1; // 1 = space
        size_t functionNameEndIndex = symbol.find(" + ");
        size_t functionNameLength = functionNameEndIndex - functionNameStartIndex;

        // Extract the mangled function name
        ::std::string functionName = symbol.substr(functionNameStartIndex, functionNameLength);

        // Demangling status:
        // [0] The demangling operation succeeded
        // [-1] A memory allocation failiure occurred
        // [-2] mangled_name is not a valid name under the C++ ABI mangling rules
        // [-3] One of the arguments is invalid
        int demanglingStatus = -1;

        // A region of memory, allocated with malloc, of *length bytes, into which the demangled name is stored.
        // May be NULL; in that case, the demangled name is placed in a region of memory allocated with malloc.
        char* outputBuffer = nullptr;

        // If length is non-NULL, the length of the buffer containing the demangled name is placed in *length.
        size_t* length = nullptr;

        // https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html
        char* demangledFunctionName = abi::__cxa_demangle(functionName.c_str(), outputBuffer, length, &demanglingStatus);

        bool successfullyDemangled = demanglingStatus == 0 && demangledFunctionName != nullptr;
        if (successfullyDemangled) {
            symbol.replace(functionNameStartIndex, functionNameLength, demangledFunctionName);
        }

        free(demangledFunctionName);
    }
#endif
}