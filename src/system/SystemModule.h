#pragma once

#include "AppConfig.h"
#include "FileSystem.h"

namespace Blink {
    class SystemModule {
    private:
        FileSystem* fileSystem;

    public:
        SystemModule();

        ~SystemModule();

        FileSystem* getFileSystem() const;

        bool initialize(const AppConfig& appConfig) const;

        void terminate() const;
    };
}
