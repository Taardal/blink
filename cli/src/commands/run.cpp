#include "pch.h"
#include "commands.h"

namespace BlinkCLI {

    CLI::Command run() {
        CLI::Option releaseOption;
        releaseOption.Name = "release";
        releaseOption.Usage = "Use Release mode";
        releaseOption.Aliases = {"r"};

        CLI::Command command;
        command.Name = "run";
        command.Usage = "Run project";
        command.Aliases = {"r"};
        command.Options = {
                releaseOption,
        };
        command.Action = [](const CLI::Context& context) -> void {
            const char* binDirName = "bin";
            const char* buildTypeDirName = context.hasOption("release") ? "release" : "debug";
            std::string_view appName = context.App->Name;

            std::stringstream ss;
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