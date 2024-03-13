#include "LuaModule.h"

namespace Blink {
    LuaModule::LuaModule(WindowModule* windowModule)
        : luaEngine(new LuaEngine(windowModule->keyboard)) {
    }

    LuaModule::~LuaModule() {
        delete luaEngine;
    }

    bool LuaModule::initialize() {
        return luaEngine->initialize();
    }

    void LuaModule::terminate() {
        luaEngine->terminate();
    }
}
