#include "pch.h"
#include "commands.h"

namespace BlinkCLI {

    CLI::Command shaders() {
        CLI::Option releaseOption;
        releaseOption.Name = "release";
        releaseOption.Usage = "Use Release mode";
        releaseOption.Aliases = {"r"};

        CLI::Command command;
        command.Name = "shaders";
        command.Usage = "Compile shaders";
        command.Options = {
                releaseOption
        };
        command.Action = [](const CLI::Context& context) -> void {
            const char* resourcesDirectoryName = "res";
            const char* shaderDirectoryName = "shaders";

            const char* binDirectoryName = "bin";
            const char* buildTypeDirectoryName = context.hasOption("release") ? "release" : "debug";

            const char* cmakeScriptsDirectoryName = "cmake";
            const char* cmakeScriptName = "compile_shaders.cmake";

            std::stringstream ss;
            ss << "cmake";
            ss << " -DSHADER_SRC_DIR=" << resourcesDirectoryName << "/" << shaderDirectoryName;
            ss << " -DSHADER_DST_DIR=" << binDirectoryName << "/" << buildTypeDirectoryName << "/" << shaderDirectoryName;
            ss << " -P " << cmakeScriptsDirectoryName << "/" << cmakeScriptName;
            std::string command = ss.str();

            std::system(command.c_str());
        };
        return command;
    }
}
