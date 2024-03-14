#include "SystemModule.h"

namespace Blink {
    SystemModule::SystemModule()
            : fileSystem(new FileSystem()) {}

    SystemModule::~SystemModule() {
        delete fileSystem;
    }

    bool SystemModule::initialize(const AppConfig& appConfig) const {
        Log::SetLevel(appConfig.logLevel);
        return true;
    }

    void SystemModule::terminate() const {
    }
}
