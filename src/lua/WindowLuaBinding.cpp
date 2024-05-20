#include "WindowLuaBinding.h"

namespace Blink {
    WindowLuaBinding::WindowLuaBinding(Window* window) : window(window) {
    }

    void WindowLuaBinding::initialize(lua_State* L, Window* window) {
        std::string typeName = "Window";
        std::string metatableName = typeName + "__meta";

        // Allocate memory for the C++ object and push a userdata onto the Lua stack
        void* userdata = lua_newuserdata(L, sizeof(WindowLuaBinding));

        // Construct the C++ object in the allocated memory block
        new(userdata) WindowLuaBinding(window);

        // Create a new metatable and push it onto the Lua stack
        luaL_newmetatable(L, metatableName.c_str());

        // Set the __gc metamethod of the metatable to binding destroy function
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, WindowLuaBinding::destroy);
        lua_settable(L, -3);

        // Set the __index metamethod of the metatable to binding index function
        lua_pushstring(L, "__index");
        constexpr int upvalueCount = 0;
        lua_pushcclosure(L, WindowLuaBinding::index, upvalueCount);
        lua_settable(L, -3);

        // Set the newly created metatable as the metatable of the userdata
        lua_setmetatable(L, -2);

        // Create Lua object to represent the userdata (C++ object) in Lua code
        lua_newtable(L);

        // Associate the Lua object with the userdata (C++ object) by assigning the Lua object as the userdata's user value
        lua_setuservalue(L, -2);

        // Create a global Lua variable and associate the userdata (C++ object) with it
        lua_setglobal(L, typeName.c_str());
    }

    // Lua stack
    // - [-1] userdata  Binding object
    int WindowLuaBinding::destroy(lua_State* L) {
        auto* binding = (WindowLuaBinding*) lua_touserdata(L, -1);
        binding->~WindowLuaBinding();
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] userdata  Binding object
    int WindowLuaBinding::index(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "getAspectRatio") {
            lua_pushcfunction(L, WindowLuaBinding::getAspectRatio);
            return 1;
        }
        BL_LOG_WARN("Could not resolve index [{}]", indexName);
        return 0;
    }

    // Lua stack
    // - [-1] userdata  Binding object
    int WindowLuaBinding::getAspectRatio(lua_State* L) {
        auto* binding = (WindowLuaBinding*) lua_touserdata(L, -1);
        lua_pushnumber(L, binding->window->getAspectRatio());
        return 1;
    }
}
