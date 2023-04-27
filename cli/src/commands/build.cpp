#include "pch.h"
#include "commands.h"

namespace BlinkCLI {

    CLI::Command build() {
        CLI::Option buildDirectoryOption;
        buildDirectoryOption.Name = "buildDir";
        buildDirectoryOption.Usage = "Where to store build files generated with CMake";
        buildDirectoryOption.DefaultValue = "build";
        buildDirectoryOption.Aliases = {"d"};

        CLI::Option cmakeSourceDirectoryOption;
        cmakeSourceDirectoryOption.Name = "cmakeDir";
        cmakeSourceDirectoryOption.Usage = "Where the CMake source files are located";
        cmakeSourceDirectoryOption.DefaultValue = ".";
        cmakeSourceDirectoryOption.Aliases = {"c"};

        CLI::Option glfwOption;
        glfwOption.Name = "glfw";
        glfwOption.Usage = "Build GLFW as part of this project instead of using binaries installed on local machine";
        glfwOption.Aliases = {"g"};

        CLI::Option releaseOption;
        releaseOption.Name = "release";
        releaseOption.Usage = "Use Release mode";
        releaseOption.Aliases = {"r"};

        CLI::Command command;
        command.Name = "build";
        command.Usage = "Build project";
        command.Options = {
                buildDirectoryOption,
                cmakeSourceDirectoryOption,
                glfwOption,
                releaseOption
        };
        command.Action = [](const CLI::Context& context) -> void {
            std::string_view buildType = context.hasOption("release") ? "Release" : "Debug";
            std::string_view buildDirectory = context.hasOption("buildDir") ? context.getOption("buildDir")->Value : context.Command->getOption("buildDir")->DefaultValue;
            std::string_view cmakeSourceDirectory = context.hasOption("cmakeDir") ? context.getOption("cmakeDir")->Value : context.Command->getOption("cmakeDir")->DefaultValue;

            std::stringstream ss;
            ss << "cmake -DCMAKE_BUILD_TYPE=" << buildType;
            if (context.hasOption("glfw")) {
                ss << " -DBUILD_GLFW_SRC=ON";
            }
            ss << " -B " << buildDirectory;
            ss << " -S " << cmakeSourceDirectory;
            ss << " && ";
            ss << "cmake --build " << buildDirectory << " --config " << buildType;

            std::string command = ss.str();
            printf("%s\n", command.c_str());
            std::system(command.c_str());
        };
        return command;
    }
}