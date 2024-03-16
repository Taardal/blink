#include "pch.h"
#include "commands.h"

namespace BlinkCLI {
    CLI::Command generate() {
        CLI::Option generatorOption;
        generatorOption.Name = "generator";
        generatorOption.Usage = "Which CMake generator to use";
        generatorOption.Aliases = { "g" };

        CLI::Option useNinjaGeneratorOption;
        useNinjaGeneratorOption.Name = "ninja";
        useNinjaGeneratorOption.Usage = "Use Ninja as CMake generator (overrides 'generator' option)";
        useNinjaGeneratorOption.Aliases = { "n" };

        CLI::Option buildTypeOption;
        buildTypeOption.Name = "buildType";
        buildTypeOption.Usage = "Which CMake build type to use";
        buildTypeOption.DefaultValue = "Debug";
        buildTypeOption.Aliases = { "t" };

        CLI::Option useReleaseBuildTypeOption;
        useReleaseBuildTypeOption.Name = "release";
        useReleaseBuildTypeOption.Usage = "Use Release as CMake build type (overrides 'buildType' option)";
        useReleaseBuildTypeOption.Aliases = { "r" };

        CLI::Command command;
        command.Name = "generate";
        command.Usage = "Generate project files using CMake";
        command.Aliases = { "gen" };
        command.Options = {
            generatorOption,
            useNinjaGeneratorOption,
            buildTypeOption,
            useReleaseBuildTypeOption
        };
        command.Action = [](const CLI::Context& context) -> void {
            const CLI::Option* generatorOption = context.Command->getOption("generator");
            const std::string_view generator = context.hasOption("ninja") ? "Ninja" : generatorOption->getValueOrDefault();

            const CLI::Option* buildTypeOption = context.Command->getOption("buildType");
            const std::string_view buildType = context.hasOption("release") ? "Release" : buildTypeOption->getValueOrDefault();

            std::stringstream ss;
            ss << "cmake";
            ss << " -D CMAKE_BUILD_TYPE=" << buildType;
            if (generator.length() > 0) {
                ss << " -D GENERATOR=" << generator;
            }
            ss << " -P cmake/generate.cmake";

            const std::string systemCommand = ss.str();
            std::system(systemCommand.c_str());
        };
        return command;
    }
}
