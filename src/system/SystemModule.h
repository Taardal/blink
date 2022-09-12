#pragma once

#include "Config.h"
#include "FileSystem.h"

namespace Blink {
    class SystemModule {
    private:
        FileSystem* fileSystem;

    public:
        SystemModule();

        ~SystemModule();

        FileSystem* getFileSystem() const;

        bool initialize(const Config& config) const;

        void terminate() const;
    };
}
