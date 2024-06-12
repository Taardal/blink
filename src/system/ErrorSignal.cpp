#include "ErrorSignal.h"

namespace Blink {
    ErrorSignal::ErrorSignal(int signal) : code(signal) {
        if (signal == SIGHUP) {
            name = "SIGHUP";
            description = "Hangup. Typically sent to a process when its controlling terminal is closed. Often used to reload configurations.";
        }
        if (signal == SIGINT) {
            name = "SIGINT";
            description = "Interrupt. Sent when the user types the interrupt character (usually Ctrl+C).";
        }
        if (signal == SIGQUIT) {
            name = "SIGQUIT";
            description = "Quit. Sent when the user types the quit character (usually Ctrl+).";
        }
        if (signal == SIGILL) {
            name = "SIGILL";
            description = "Illegal instruction. Indicates that the process has attempted to execute an illegal, malformed, or privileged instruction.";
        }
        if (signal == SIGTRAP) {
            name = "SIGTRAP";
            description = "Trace trap. Used by debuggers to handle breakpoint traps and other traps.";
        }
        if (signal == SIGABRT) {
            name = "SIGABRT";
            description = "Abort. Sent by the abort() function.";
        }
        if (signal == SIGFPE) {
            name = "SIGFPE";
            description = "Floating-point exception. Indicates an erroneous arithmetic operation, such as division by zero or an overflow.";
        }
        if (signal == SIGKILL) {
            name = "SIGKILL";
            description = "Kill. This signal cannot be caught or ignored.";
        }
        if (signal == SIGBUS) {
            name = "SIGBUS";
            description = "Bus error. Indicates an access to an invalid address.";
        }
        if (signal == SIGSEGV) {
            name = "SIGSEGV";
            description = "Segmentation violation. Indicates an invalid access to storage.";
        }
        if (signal == SIGSYS) {
            name = "SIGSYS";
            description = "Bad system call. Indicates an invalid system call.";
        }
        if (signal == SIGPIPE) {
            name = "SIGPIPE";
            description = "Broken pipe. Sent when a process writes to a pipe without a reader.";
        }
        if (signal == SIGALRM) {
            name = "SIGALRM";
            description = "Alarm clock. Sent when a timer set by alarm() expires.";
        }
        if (signal == SIGTERM) {
            name = "SIGTERM";
            description = "Termination. Sent to request termination.";
        }
        if (signal == SIGXCPU) {
            name = "SIGXCPU";
            description = "CPU time limit exceeded.";
        }
        if (signal == SIGXFSZ) {
            name = "SIGXFSZ";
            description = "File size limit exceeded.";
        }
    }

    void ErrorSignal::printStacktrace() const {
        printStacktrace(*this);
    }
}

// --------------------------------------------------------------------------------------------------------------
// STATIC
// --------------------------------------------------------------------------------------------------------------

namespace Blink {
    std::function<void(const ErrorSignal&)> ErrorSignal::handlerFn;

    void ErrorSignal::setHandler(const ::std::function<void(const ErrorSignal&)>& handler) {
        handlerFn = handler;

        signal(SIGHUP, handleErrorSignal);
        signal(SIGINT, handleErrorSignal);
        signal(SIGQUIT, handleErrorSignal);
        signal(SIGILL, handleErrorSignal);
        signal(SIGTRAP, handleErrorSignal);
        signal(SIGABRT, handleErrorSignal);
        signal(SIGEMT, handleErrorSignal);
        signal(SIGFPE, handleErrorSignal);
        signal(SIGKILL, handleErrorSignal);
        signal(SIGBUS, handleErrorSignal);
        signal(SIGSEGV, handleErrorSignal);
        signal(SIGSYS, handleErrorSignal);
        signal(SIGPIPE, handleErrorSignal);
        signal(SIGALRM, handleErrorSignal);
        signal(SIGTERM, handleErrorSignal);
        signal(SIGXCPU, handleErrorSignal);
        signal(SIGXFSZ, handleErrorSignal);
    }

    void ErrorSignal::handleErrorSignal(int signal) {
        ErrorSignal s(signal);
        handlerFn(s);
    }

    void ErrorSignal::printStacktrace(const ErrorSignal& signal) {
        fprintf(stderr, "--------------------------------------------------------------------------------------------------------------\n");
        fprintf(stderr, "[%s] %s\n", signal.name.c_str(), signal.description.c_str());
        fprintf(stderr, "--------------------------------------------------------------------------------------------------------------\n");
#if defined(BL_PRINT_UNIX_STACKTRACE)
        printUnixStacktrace();
#elif defined(BL_PRINT_WINDOWS_STACKTRACE)
        fprintf(stderr, "Could not print stacktrace for Windows, not implemented\n");
#else
        fprintf(stderr, "Could not print stacktrace, unsupported platform\n");
#endif
    }

#ifdef BL_PRINT_UNIX_STACKTRACE

#include <cxxabi.h>
#include <execinfo.h>
#include <unistd.h>

    void ErrorSignal::printUnixStacktrace() {
        // Number of stacktrace lines to be printed
        constexpr int maxStackSize = 30;

        // Linux man page: backtrace, backtrace_symbols, backtrace_symbols_fd
        // https://linux.die.net/man/3/backtrace_symbols

        // backtrace() returns a backtrace for the calling program.
        // A backtrace is the series of currently active function calls for the program.
        // Each item is of type void*, and is the return address from the corresponding stack frame
        void* stack[maxStackSize];
        int stackSize = backtrace(stack, maxStackSize);

        // Given the set of addresses returned by backtrace() in buffer,
        // backtrace_symbols() translates the addresses into an array of strings that describe the addresses symbolically
        char** stacktrace = backtrace_symbols(stack, stackSize);
        if (stacktrace == nullptr) {
            fprintf(stderr, "Could not resolve stacktrace\n");
            return;
        }

        // Iterate over the backtrace and...
        // - Convert the line to a std::string
        // - Demangle the line
        // - Print the line
        for (int i = 0; i < stackSize; i++) {
            ::std::string stacktraceLine(stacktrace[i]);
            demangleUnixStacktraceLine(&stacktraceLine);
            fprintf(stderr, "%s\n", stacktraceLine.c_str());
        }

        // This array is malloced by backtrace_symbols(), and must be freed by the caller
        free(stacktrace);
    }

    // "Mangling" is a technique used by compilers to encode additional information into symbol names in compiled code.
    // This additional information includes things like function names, namespaces, argument types, and template parameters.
    // We want to "demangle" (i.e. decode) this information to make it human-readable.
    //
    // Mangled stacktrace line:
    //   10  blink  0x0000000102cbfe5c  _ZN5Blink3App10initializeEv + 1960
    //
    // Demangled stacktrace line:
    //   10  blink  0x00000001028b3e50  Blink::App::initialize() + 1960
    //
    // Segments:
    //   [stack index]  [binary name]  [return address (in hexadecimal)]  [function name] + [offset into the function (in hexadecimal)]
    void ErrorSignal::demangleUnixStacktraceLine(::std::string* stacktraceLine) {
        // Find segment before the mangled function name
        size_t returnAddressStartIndex = stacktraceLine->find("0x");
        size_t returnAddressEndIndex = returnAddressStartIndex + 18; // 18 = return address length

        // Find function name start and end
        size_t functionNameStartIndex = returnAddressEndIndex + 1; // 1 = space
        size_t functionNameEndIndex = stacktraceLine->find(" + ");
        size_t functionNameLength = functionNameEndIndex - functionNameStartIndex;

        // Extract the mangled function name
        ::std::string functionName = stacktraceLine->substr(functionNameStartIndex, functionNameLength);

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
        char* demangledFunctionName =
                abi::__cxa_demangle(functionName.c_str(), outputBuffer, length, &demanglingStatus);

        // Replace the mangled function name with the demangled one in the stacktrace line
        bool successfullyDemangled = demanglingStatus == 0 && demangledFunctionName != nullptr;
        if (successfullyDemangled) {
            stacktraceLine->replace(functionNameStartIndex, functionNameLength, demangledFunctionName);
        }

        // If the output buffer is NULL, the demangled name is placed in a region of memory allocated with malloc.
        // The caller is responsible for deallocating this memory using free.
        free(demangledFunctionName);
    }
#endif
}
