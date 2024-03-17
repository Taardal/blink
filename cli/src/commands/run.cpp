#include "pch.h"
#include "commands.h"

namespace BlinkCLI {
    CLI::Command run() {
        CLI::Option buildOption;
        buildOption.name = "build";
        buildOption.usage = "Also run 'build' command to build binary using CMake.";
        buildOption.aliases = { "b" };

        CLI::Option buildTypeOption;
        buildTypeOption.name = "buildType";
        buildTypeOption.usage = "Which CMake build type to use. Used with 'build' option.";
        buildTypeOption.aliases = { "t" };
        buildTypeOption.defaultValue = "Debug";

        CLI::Option useReleaseBuildTypeOption;
        useReleaseBuildTypeOption.name = "release";
        useReleaseBuildTypeOption.usage = "Use Release as CMake build type. Used with 'build' option. Overrides 'buildType' option.";
        useReleaseBuildTypeOption.aliases = { "r" };

        CLI::Command command;
        command.name = "run";
        command.usage = "Run project";
        command.options = {
            buildOption,
            buildTypeOption,
            useReleaseBuildTypeOption,
        };
        command.action = [](const CLI::Context& context) -> void {
            if (context.hasOption("build")) {
                context.app->runCommand("build", context);
            }

            std::string buildType;
            if (context.hasOption("release")) {
                buildType = "Release";
            } else if (context.hasOption("buildType")) {
                buildType = context.getOption("buildType")->value;
            } else {
                buildType = context.command->getOption("buildType")->defaultValue;
            }
            std::string buildTypeDirectoryName;
            buildTypeDirectoryName.reserve(buildType.size());
            for (char character : buildType) {
                buildTypeDirectoryName.push_back(std::tolower(character));
            }

            std::string blinkRootDirectory = std::filesystem::current_path();
            std::filesystem::current_path(blinkRootDirectory + "/bin/" + buildTypeDirectoryName);

            std::stringstream ss;
#ifdef _WIN32
            ss << "start blink.exe"; // Windows
#else
            ss << "./blink"; // Linux and macOS
#endif
            std::string systemCommand = ss.str();
            std::system(systemCommand.c_str());
        };
        return command;
    }
}
