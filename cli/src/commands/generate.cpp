#include "pch.h"
#include "commands.h"

namespace BlinkCLI {
    CLI::Command generate() {
        CLI::Option buildTypeOption;
        buildTypeOption.name = "buildType";
        buildTypeOption.usage = "Which CMake build type to use.";
        buildTypeOption.aliases = { "t" };
        buildTypeOption.defaultValue = "Debug";

        CLI::Option useReleaseBuildTypeOption;
        useReleaseBuildTypeOption.name = "release";
        useReleaseBuildTypeOption.usage = "Use Release as CMake build type. Overrides 'buildType' option.";
        useReleaseBuildTypeOption.aliases = { "r" };

        CLI::Option generatorOption;
        generatorOption.name = "generator";
        generatorOption.usage = "Which CMake generator to use.";
        generatorOption.aliases = { "g" };

        CLI::Option useNinjaGeneratorOption;
        useNinjaGeneratorOption.name = "ninja";
        useNinjaGeneratorOption.usage = "Use Ninja as CMake generator. Overrides 'generator' option.";
        useNinjaGeneratorOption.aliases = { "n" };

        CLI::Command command;
        command.name = "generate";
        command.usage = "Generate project files using CMake";
        command.aliases = { "gen" };
        command.options = {
            buildTypeOption,
            useReleaseBuildTypeOption,
            generatorOption,
            useNinjaGeneratorOption
        };
        command.action = [](const CLI::Context& context) -> void {
            std::string generator;
            if (context.hasOption("ninja")) {
                generator = "Ninja";
            } else if (context.hasOption("generator")) {
                generator = context.getOption("generator")->value;
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

            std::stringstream ss;
            ss << "cmake";
            ss << " -D CMAKE_EXPORT_COMPILE_COMMANDS=1";
            ss << " -D CMAKE_BUILD_TYPE=" << buildType;
            ss << " -S .";
            ss << " -B build/" << buildTypeDirectoryName;
            if (generator.length() > 0) {
                ss << " -G " << generator;
            }
            std::string systemCommand = ss.str();
            std::system(systemCommand.c_str());
        };
        return command;
    }
}
