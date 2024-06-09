#include "MouseLuaBinding.h"

#include "luaUtils.h"

namespace Blink {
    MouseLuaBinding::MouseLuaBinding(Mouse* mouse): mouse(mouse) {
    }

    void MouseLuaBinding::initialize(lua_State* L, Mouse* mouse) {
        createMouseTable(L, mouse);
        createMouseButtonTable(L, mouse);
    }

    void MouseLuaBinding::createMouseTable(lua_State* L, Mouse* mouse) {
        std::string typeName = "Mouse";
        std::string metatableName = typeName + "__meta";

        // Allocate memory for the C++ object and push a userdata onto the Lua stack
        void* userdata = lua_newuserdata(L, sizeof(MouseLuaBinding));

        // Construct the C++ object in the allocated memory block
        new(userdata) MouseLuaBinding(mouse);

        // Create a new metatable and push it onto the Lua stack
        luaL_newmetatable(L, metatableName.c_str());
        // Set the __gc metamethod of the metatable to MouseBinding::destroy
        {
            lua_pushstring(L, "__gc");
            lua_pushcfunction(L, MouseLuaBinding::destroy);
            lua_settable(L, -3);
        }
        // Set the __index metamethod of the metatable to MouseBinding::index
        {
            lua_pushstring(L, "__index");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, MouseLuaBinding::index, upvalueCount);
            lua_settable(L, -3);
        }
        // Set the newly created metatable as the metatable of the userdata
        lua_setmetatable(L, -2);

        // Create Lua object
        lua_newtable(L);

        // Associate the Lua table with the userdata (C++ object) as its user value
        lua_setuservalue(L, -2);

        // Associate the mouse to the userdata as a light user value (C++ pointer)
        lua_pushlightuserdata(L, mouse);
        lua_setuservalue(L, -2);

        // Create a global Lua variable and assign the userdata to it (C++ object)
        lua_setglobal(L, typeName.c_str());
    }

    void MouseLuaBinding::createMouseButtonTable(lua_State* L, Mouse* mouse) {
        lua_newtable(L);
        for (std::pair<const std::string&, MouseButton> buttonPair : mouse->getButtonsByName()) {
            lua_pushstring(L, buttonPair.first.c_str());
            lua_pushnumber(L, (uint16_t) buttonPair.second);
            lua_settable(L, -3);
        }
        lua_setglobal(L, "MouseButton");
    }

    // Lua stack
    // - [-1] userdata  Binding
    int MouseLuaBinding::destroy(lua_State* L) {
        auto* binding = (MouseLuaBinding*) lua_touserdata(L, -1);
        binding->~MouseLuaBinding();
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] userdata  Binding
    int MouseLuaBinding::index(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "isPressed") {
            lua_pushcfunction(L, MouseLuaBinding::isPressed);
            return 1;
        }
        BL_LOG_WARN("Could not resolve index [{}]", indexName);
        return 0;
    }

    // Lua stack
    // - [-1] number/string MouseButton to check, given by either value or name
    // - [-2] userdata      Binding
    int MouseLuaBinding::isPressed(lua_State* L) {
        auto* binding = (MouseLuaBinding*) lua_touserdata(L, -2);
        bool pressed;
        if (lua_type(L, -1) == LUA_TSTRING) {
            std::string button = lua_tostring(L, -1);
            pressed = binding->mouse->isPressed(button);
        } else {
            uint16_t button = lua_tonumber(L, -1);
            pressed = binding->mouse->isPressed(button);
        }
        lua_pushboolean(L, pressed);
        return 1;
    }
}
