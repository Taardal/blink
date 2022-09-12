#include "commands/BuildProjectCommand.h"
#include "commands/RunProjectCommand.h"
#include "commands/CopyResourcesCommand.h"
#include <cli.h>

int main(int argc, char* argv[]) {
    CLI::App app;
    app.Name = "blink";
    app.Usage = "Blink CLI";
    app.Commands = {
            BlinkCLI::buildProject(),
            BlinkCLI::runProject(),
            BlinkCLI::copyResources()
    };
    app.Run(argc, argv);
    return 0;
}