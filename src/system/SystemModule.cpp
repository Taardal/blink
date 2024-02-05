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

    bool SystemModule::initialize(const AppConfig& appConfig) const {
        Log::SetLevel(appConfig.logLevel);
        return true;
    }

    void SystemModule::terminate() const {
    }
}
