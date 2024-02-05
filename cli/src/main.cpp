#include "commands/commands.h"
#include <cli.h>

int main(int argc, char* argv[]) {
    CLI::App app;
    app.Name = "blink";
    app.Usage = "Blink CLI";
    app.Commands = {
            BlinkCLI::build(),
            BlinkCLI::generate(),
            BlinkCLI::run(),
            BlinkCLI::shaders(),
    };
    app.Run(argc, argv);
    return 0;
}