#include "commands/commands.h"
#include <cli.h>

int main(int argc, char* argv[]) {
    CLI::App app;
    app.name = "blink";
    app.usage = "Blink CLI";
    app.commands = {
            BlinkCLI::generate(),
            BlinkCLI::build(),
            BlinkCLI::run(),
            BlinkCLI::compileLua(),
            BlinkCLI::compileShaders(),
    };
    app.run(argc, argv);
    return 0;
}