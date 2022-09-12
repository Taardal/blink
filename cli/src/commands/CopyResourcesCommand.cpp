#include "CopyResourcesCommand.h"
#include <sstream>
#include <string>

namespace BlinkCLI {
    CLI::Command copyResources() {
        CLI::Option releaseOption;
        releaseOption.Name = "release";
        releaseOption.Usage = "Use Release mode";
        releaseOption.Aliases = {"r"};

        CLI::Command command;
        command.Name = "res";
        command.Usage = "Copy resources to bin directory";
        command.Options = {
                releaseOption
        };
        command.Action = [](const CLI::Context& context) -> void {
            const char* buildType = context.hasOption("release") ? "Release" : "Debug";

            const char* resDirectoryName = "res";
            const char* binDirectoryName = "bin";
            const char* cmakeScriptPath = "cmake/res.cmake";

            std::stringstream ss;
            ss << "cmake";
            ss << " -D RES_DIR=" << resDirectoryName;
            ss << " -D BIN_DIR=" << binDirectoryName;
            ss << " -D CMAKE_BUILD_TYPE=" << buildType;
            ss << " -P " << cmakeScriptPath;
            std::string command = ss.str();

            std::system(command.c_str());
        };
        return command;
    }
}