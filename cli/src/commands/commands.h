#pragma once

#include <cli.h>

namespace BlinkCLI {
    CLI::Command generate();
    CLI::Command build();
    CLI::Command run();
    CLI::Command compileLua();
    CLI::Command compileShaders();
}