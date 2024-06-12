#include "Signal.h"

namespace Blink {
    Signal Signal::create(int signal) {
        Signal s = {};
        s.code = signal;
        if (signal == SIGHUP) {
            s.name = "SIGHUP";
            s.description = "Hangup. Typically sent to a process when its controlling terminal is closed. Often used to reload configurations.";
        }
        if (signal == SIGINT) {
            s.name = "SIGINT";
            s.description = "Interrupt. Sent when the user types the interrupt character (usually Ctrl+C).";
        }
        if (signal == SIGQUIT) {
            s.name = "SIGQUIT";
            s.description = "Quit. Sent when the user types the quit character (usually Ctrl+).";
        }
        if (signal == SIGILL) {
            s.name = "SIGILL";
            s.description = "Illegal instruction. Indicates that the process has attempted to execute an illegal, malformed, or privileged instruction.";
        }
        if (signal == SIGTRAP) {
            s.name = "SIGTRAP";
            s.description = "Trace trap. Used by debuggers to handle breakpoint traps and other traps.";
        }
        if (signal == SIGABRT) {
            s.name = "SIGABRT";
            s.description = "Abort. Sent by the abort() function.";
        }
#if  (defined(_POSIX_C_SOURCE) && !defined(_DARWIN_C_SOURCE))
        if (signal == SIGPOLL) {
            s.name = "SIGPOLL";
            s.description = "Pollable event.";
        }
#else
        if (signal == SIGEMT) {
            s.name = "SIGEMT";
            s.description = "EMT instruction.";
        }
#endif
        if (signal == SIGFPE) {
            s.name = "SIGFPE";
            s.description = "Floating-point exception. Indicates an erroneous arithmetic operation, such as division by zero or an overflow.";
        }
        if (signal == SIGKILL) {
            s.name = "SIGKILL";
            s.description = "Kill. This signal cannot be caught or ignored.";
        }
        if (signal == SIGBUS) {
            s.name = "SIGBUS";
            s.description = "Bus error. Indicates an access to an invalid address.";
        }
        if (signal == SIGSEGV) {
            s.name = "SIGSEGV";
            s.description = "Segmentation violation. Indicates an invalid access to storage.";
        }
        if (signal == SIGSYS) {
            s.name = "SIGSYS";
            s.description = "Bad system call. Indicates an invalid system call.";
        }
        if (signal == SIGPIPE) {
            s.name = "SIGPIPE";
            s.description = "Broken pipe. Sent when a process writes to a pipe without a reader.";
        }
        if (signal == SIGALRM) {
            s.name = "SIGALRM";
            s.description = "Alarm clock. Sent when a timer set by alarm() expires.";
        }
        if (signal == SIGTERM) {
            s.name = "SIGTERM";
            s.description = "Termination. Sent to request termination.";
        }
        if (signal == SIGURG) {
            s.name = "SIGURG";
            s.description = "Urgent condition on I/O channel.";
        }
        if (signal == SIGSTOP) {
            s.name = "SIGSTOP";
            s.description = "Stop. This signal cannot be caught or ignored.";
        }
        if (signal == SIGTSTP) {
            s.name = "SIGTSTP";
            s.description = "Terminal stop. Sent when the user types the stop character (usually Ctrl+Z).";
        }
        if (signal == SIGCONT) {
            s.name = "SIGCONT";
            s.description = "Continue. Sent to continue a process that was stopped.";
        }
        if (signal == SIGCHLD) {
            s.name = "SIGCHLD";
            s.description = "Child status has changed. Sent to a parent process when a child process terminates or stops.";
        }
        if (signal == SIGTTIN) {
            s.name = "SIGTTIN";
            s.description = "Background read from tty. Sent to a background process trying to read from the terminal.";
        }
        if (signal == SIGTTOU) {
            s.name = "SIGTTOU";
            s.description = "Background write to tty. Sent to a background process trying to write to the terminal.";
        }
#if  (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE))
        if (signal == SIGIO) {
            s.name = "SIGIO";
            s.description = "I/O now possible.";
        }
#endif
        if (signal == SIGXCPU) {
            s.name = "SIGXCPU";
            s.description = "CPU time limit exceeded.";
        }
        if (signal == SIGXFSZ) {
            s.name = "SIGXFSZ";
            s.description = "File size limit exceeded.";
        }
        if (signal == SIGVTALRM) {
            s.name = "SIGVTALRM";
            s.description = "Virtual timer expired.";
        }
        if (signal == SIGPROF) {
            s.name = "SIGPROF";
            s.description = "Profiling timer expired.";
        }
#if  (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE))
        if (signal == SIGWINCH) {
            s.name = "SIGWINCH";
            s.description = "Window size change.";
        }
        if (signal == SIGINFO) {
            s.name = "SIGINFO";
            s.description = "Information request.";
        }
#endif
        if (signal == SIGUSR1) {
            s.name = "SIGUSR1";
            s.description = "User-defined signal 1.";
        }
        if (signal == SIGUSR2) {
            s.name = "SIGUSR2";
            s.description = "User-defined signal 2.";
        }
        return s;
    }

    void Signal::printStacktrace() const {
        Signal::printStacktrace(*this);
    }

    void Signal::printStacktrace(const Signal& signal) {
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
}

namespace Blink {

    std::map<int, std::vector<std::function<void(int)>>> SignalHandler::handlers;
    std::function<void(const Signal&)> SignalHandler::errorHandler;

    void SignalHandler::addHandler(int signal, const std::function<void(int)>& handler) {
        handlers[signal].push_back(handler);
    }

    void SignalHandler::setErrorSignalHandler(const ::std::function<void(const Signal&)>& handler) {
        errorHandler = handler;

        auto foo = [](int signal) {
            errorHandler(Signal::create(signal));
        };

        signal(SIGHUP, foo);
        signal(SIGINT, onErrorSignal);
        signal(SIGQUIT, onErrorSignal);
        signal(SIGILL, onErrorSignal);
        signal(SIGTRAP, onErrorSignal);
        signal(SIGABRT, onErrorSignal);
        signal(SIGEMT, onErrorSignal);
        signal(SIGFPE, onErrorSignal);
        signal(SIGKILL, onErrorSignal);
        signal(SIGBUS, onErrorSignal);
        signal(SIGSEGV, onErrorSignal);
        signal(SIGSYS, onErrorSignal);
        signal(SIGPIPE, onErrorSignal);
        signal(SIGALRM, onErrorSignal);
        signal(SIGTERM, onErrorSignal);
        signal(SIGXCPU, onErrorSignal);
        signal(SIGXFSZ, onErrorSignal);
    }

    void SignalHandler::onErrorSignal(int signal) {
        errorHandler(Signal::create(signal));
    }

    void SignalHandler::initialize() {
        // signal(SIGTRAP, onSignal);
        //
        // signal(SIGHUP, onSignal); //(1): Hangup. Default action is to terminate the process.
        // signal(SIGINT, onSignal); //(2): Interrupt. Default action is to terminate the process.
        // signal(SIGQUIT, onSignal); //(3): Quit. Default action is to terminate the process and produce a core dump.
        // signal(SIGILL, onSignal); //(4): Illegal instruction. Default action is to terminate the process and produce a core dump.
        // signal(SIGTRAP, onSignal); //(5): Trace trap. Default action is to terminate the process and produce a core dump.
        // signal(SIGABRT, onSignal); //(6): Abort. Default action is to terminate the process and produce a core dump.
        // signal(SIGEMT, onSignal); //(7): EMT instruction (system-specific). Default action is to terminate the process and produce a core dump.
        // signal(SIGFPE, onSignal); //(8): Floating-point exception. Default action is to terminate the process and produce a core dump.
        // signal(SIGKILL, onSignal); //(9): Kill. This signal cannot be caught or ignored. It causes immediate termination of the process.
        // signal(SIGBUS, onSignal); //(10): Bus error. Default action is to terminate the process and produce a core dump.
        // signal(SIGSEGV, onSignal); //(11): Segmentation violation. Default action is to terminate the process and produce a core dump.
        // signal(SIGSYS, onSignal); //(12): Bad system call. Default action is to terminate the process and produce a core dump.
        // signal(SIGPIPE, onSignal); //(13): Broken pipe. Default action is to terminate the process.
        // signal(SIGALRM, onSignal); //(14): Alarm clock. Default action is to terminate the process.
        // signal(SIGTERM, onSignal); //(15): Termination. Default action is to terminate the process.
        // signal(SIGXCPU, onSignal); //(24): CPU time limit exceeded. Default action is to terminate the process and produce a core dump.
        // signal(SIGXFSZ, onSignal); //(25): File size limit exceeded. Default action is to terminate the process and produce a core dump.
    }

    void SignalHandler::onSignal(int signal) {
        for (auto& handler : handlers[signal]) {
            handler(signal);
        }
        if (shouldPrintStacktrace(signal)) {
            printStacktrace(signal);
        }
        if (shouldExit(signal)) {
            exit(signal);
        }
    }

    bool SignalHandler::shouldPrintStacktrace(int signal) {
        if (signal == SIGQUIT) {
            return true;
        }
        if (signal == SIGILL) {
            return true;
        }
        if (signal == SIGTRAP) {
            return true;
        }
        if (signal == SIGABRT) {
            return true;
        }
        if (signal == SIGEMT) {
            return true;
        }
        if (signal == SIGFPE) {
            return true;
        }
        if (signal == SIGBUS) {
            return true;
        }
        if (signal == SIGSEGV) {
            return true;
        }
        if (signal == SIGSYS) {
            return true;
        }
        if (signal == SIGXCPU) {
            return true;
        }
        if (signal == SIGXFSZ) {
            return true;
        }
        return false;
    }

    bool SignalHandler::shouldExit(int signal) {
        if (signal == SIGHUP) {
            return true;
        }
        if (signal == SIGINT) {
            return true;
        }
        if (signal == SIGQUIT) {
            return true;
        }
        if (signal == SIGILL) {
            return true;
        }
        if (signal == SIGTRAP) {
            return true;
        }
        if (signal == SIGABRT) {
            return true;
        }
        if (signal == SIGEMT) {
            return true;
        }
        if (signal == SIGFPE) {
            return true;
        }
        if (signal == SIGKILL) {
            return true;
        }
        if (signal == SIGBUS) {
            return true;
        }
        if (signal == SIGSEGV) {
            return true;
        }
        if (signal == SIGSYS) {
            return true;
        }
        if (signal == SIGPIPE) {
            return true;
        }
        if (signal == SIGALRM) {
            return true;
        }
        if (signal == SIGTERM) {
            return true;
        }
        if (signal == SIGXCPU) {
            return true;
        }
        if (signal == SIGXFSZ) {
            return true;
        }
        return false;
    }
}

namespace Blink {
    void addErrorSignalStacktracePrinters() {
        // (Segmentation fault): Invalid memory access.
        // signal(SIGSEGV, printStacktrace);
        //
        // // (Abort): Abnormal termination initiated by the program itself.
        // signal(SIGABRT, printStacktrace);
        //
        // // (Illegal instruction): Attempt to execute an illegal instruction.
        // signal(SIGILL, printStacktrace);
        //
        // // (Floating-point exception): Arithmetic exceptions like division by zero or floating-point overflow.
        // signal(SIGFPE, printStacktrace);
        //
        // // (Bus error): Attempt to access memory that the CPU cannot physically address.
        // signal(SIGBUS, printStacktrace);
        //
        // signal(SIGTERM, printStacktrace);
        // signal(SIGTRAP, printStacktrace);

        // signal(SIGHUP, printStacktrace);      /* hangup */
        // signal(SIGINT, printStacktrace);      /* interrupt */
        // signal(SIGQUIT, printStacktrace);     /* quit */
        // signal(SIGILL, printStacktrace);      /* illegal instruction (not reset when caught) */
        // signal(SIGTRAP, printStacktrace);     /* trace trap (not reset when caught) */
        // signal(SIGABRT, printStacktrace);     /* abort() */
        // signal(SIGFPE, printStacktrace);      /* floating point exception */
        // signal(SIGKILL, printStacktrace);     /* kill (cannot be caught or ignored) */
        // signal(SIGBUS, printStacktrace);      /* bus error */
        // signal(SIGSEGV, printStacktrace);     /* segmentation violation */
    }

    void printStacktrace(int signal) {
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
        exit(signal);
    }

    std::string getSignalName(const int signal) {
        switch (signal) {
            case SIGINT:
                return "SIGINT";
            case SIGQUIT:
                return "SIGQUIT";
            case SIGILL:
                return "SIGILL";
            case SIGTRAP:
                return "SIGTRAP";
            case SIGABRT:
                return "SIGABRT";
            case SIGFPE:
                return "SIGFPE";
            case SIGKILL:
                return "SIGKILL";
            case SIGBUS:
                return "SIGBUS";
            case SIGSEGV:
                return "SIGSEGV";
            default:
                return "";
        }
    }

#ifdef BL_PRINT_UNIX_STACKTRACE

#include <cxxabi.h>
#include <execinfo.h>
#include <unistd.h>

    void printUnixStacktrace() {
        // Number of stacktrace lines to be printed
        constexpr int maxStackSize = 15;

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
    void demangleUnixStacktraceLine(::std::string* stacktraceLine) {

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
        char* demangledFunctionName = abi::__cxa_demangle(functionName.c_str(), outputBuffer, length, &demanglingStatus);

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