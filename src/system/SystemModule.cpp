#include "SystemModule.h"
#include "Log.h"

namespace Blink {
    SystemModule::SystemModule()
            : fileSystem(new FileSystem()) {}

    SystemModule::~SystemModule() {
        delete fileSystem;
    }

    FileSystem* SystemModule::getFileSystem() const {
        return fileSystem;
    }

    bool SystemModule::initialize(const Config& config) const {
        Log::SetLevel(config.logLevel);
        return true;
    }

    void SystemModule::terminate() const {
    }
}
