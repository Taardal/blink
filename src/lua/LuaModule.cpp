#include "LuaModule.h"

namespace Blink {
    LuaModule::LuaModule() : luaEngine(new LuaEngine()) {
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
