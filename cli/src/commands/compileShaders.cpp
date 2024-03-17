#include "pch.h"
#include "commands.h"

namespace BlinkCLI {
    CLI::Command compileShaders() {
        CLI::Option buildTypeOption;
        buildTypeOption.name = "buildType";
        buildTypeOption.usage = "Which CMake build type to use";
        buildTypeOption.aliases = { "t" };
        buildTypeOption.defaultValue = "Debug";

        CLI::Option useReleaseBuildTypeOption;
        useReleaseBuildTypeOption.name = "release";
        useReleaseBuildTypeOption.usage = "Use Release as CMake build type. Overrides 'buildType' option.";
        useReleaseBuildTypeOption.aliases = { "r" };

        CLI::Command command;
        command.name = "compile:shaders";
        command.usage = "Compile shaders";
        command.options = {
            buildTypeOption,
            useReleaseBuildTypeOption
        };
        command.action = [](const CLI::Context& context) -> void {
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

            std::stringstream ss;
            ss << "cmake";
            ss << " -D SHADERS_SOURCE_DIR=res/shaders";
            ss << " -D SHADERS_OUTPUT_DIR=bin/" << buildTypeDirectoryName << "/shaders";
            ss << " -P cmake/compile_shaders.cmake";

            std::string systemCommand = ss.str();
            std::system(systemCommand.c_str());
        };
        return command;
    }
}
