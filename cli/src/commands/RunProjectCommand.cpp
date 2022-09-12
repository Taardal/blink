#include "RunProjectCommand.h"
#include <sstream>

namespace BlinkCLI {

    CLI::Command runProject() {
        CLI::Option buildOption;
        buildOption.Name = "build";
        buildOption.Usage = "Build project before running";
        buildOption.Aliases = {"b"};

        CLI::Option glfwOption;
        glfwOption.Name = "glfw";
        glfwOption.Usage = "Build GLFW as part of this project (Requires build option)";
        glfwOption.Aliases = {"g"};

        CLI::Option releaseOption;
        releaseOption.Name = "release";
        releaseOption.Usage = "Use Release mode";
        releaseOption.Aliases = {"r"};

        CLI::Command command;
        command.Name = "run";
        command.Usage = "Run project";
        command.Aliases = {"r"};
        command.Options = {
                buildOption,
                glfwOption,
                releaseOption,
        };
        command.Action = [](const CLI::Context& context) -> void {
            const char* binDirName = "bin";
            const char* buildTypeDirName = context.hasOption("release") ? "release" : "debug";
            std::string_view appName = context.App->Name;

            std::stringstream ss;
            if (context.hasOption("build")) {
                ss << "./" << context.App->Name << " build";
                if (context.hasOption("release")) {
                    ss << " --release";
                }
                if (context.hasOption("glfw")) {
                    ss << " --glfw";
                }
                ss << " && ";
            }
            ss << "cd " << binDirName << "/" << buildTypeDirName;
            ss << " && ";
            ss << "./";
            ss << appName;

            std::string command = ss.str();
            std::system(command.c_str());
        };
        return command;
    }
}