#pragma once

#include "EntityLuaBinding.h"
#include "window/Keyboard.h"

#include <lua/lua.hpp>

namespace Blink {
    class LuaEngine {
    private:
        lua_State* L;

    public:
        LuaEngine();

        ~LuaEngine();

        bool initialize();

        void terminate();

        bool loadFile(const std::string& path);

        void createEntityType(const std::string& typeName);

        void createEntityBinding(entt::registry* entityRegistry);

        void update(const std::string& tableName, double timestep, entt::entity entity);

        void createKeyboardBinding(Keyboard* keyboard) const;

    private:
        void clearStack() const;

        static int luaPrint(lua_State* L);
    };
}
