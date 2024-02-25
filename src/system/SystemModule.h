#pragma once

#include "AppConfig.h"
#include "FileSystem.h"

namespace Blink {
    struct SystemModule {
        FileSystem* fileSystem;

        SystemModule();

        ~SystemModule();

        bool initialize(const AppConfig& appConfig) const;

        void terminate() const;
    };
}
