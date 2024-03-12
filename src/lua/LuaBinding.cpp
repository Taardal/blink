#include "LuaBinding.h"

namespace Blink {
    LuaBinding::LuaBinding(lua_State* L, const std::string& tableName)  : L(L), tableName(tableName) {
    }

    void LuaBinding::update() {
        lua_getglobal(L, tableName.c_str());
        lua_getfield(L, -1, "onUpdate");
        constexpr int argumentCount = 0;
        constexpr int returnValueCount = 0;
        constexpr int messageHandlerIndex = 0;
        if (lua_pcall(L, argumentCount, returnValueCount, messageHandlerIndex) != LUA_OK) {
            BL_LOG_ERROR("Could not update lua binding [{}]: {}", tableName.c_str(), lua_tostring(L, -1));
            throw std::runtime_error("Could not update lua binding");
        }
    }
}
