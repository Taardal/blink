#include "pch.h"
#include "commands.h"

namespace BlinkCLI {

    CLI::Command build() {
        CLI::Option buildDirectoryOption;
        buildDirectoryOption.Name = "buildDirectory";
        buildDirectoryOption.Usage = "Where to store build files generated with CMake";
        buildDirectoryOption.DefaultValue = "build";
        buildDirectoryOption.Aliases = {"d", "dir"};

        CLI::Option releaseOption;
        releaseOption.Name = "release";
        releaseOption.Usage = "Use Release mode";
        releaseOption.Aliases = {"r"};

        CLI::Command command;
        command.Name = "build";
        command.Usage = "Build project";
        command.Options = {
                buildDirectoryOption,
                releaseOption
        };
        command.Action = [](const CLI::Context& context) -> void {
            std::string_view buildType = context.hasOption("release") ? "Release" : "Debug";
            std::string_view buildDirectory = context.Command->getOptionValue("buildDirectory");

            std::stringstream ss;
            ss << "cmake --build " << buildDirectory << " --config " << buildType;

            std::string command = ss.str();
            printf("%s\n", command.c_str());
            std::system(command.c_str());
        };
        return command;
    }
}
