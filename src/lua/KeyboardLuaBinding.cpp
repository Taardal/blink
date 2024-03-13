#include "KeyboardLuaBinding.h"

#include "printLua.h"

namespace Blink {
    KeyboardLuaBinding::KeyboardLuaBinding(Keyboard* keyboard): keyboard(keyboard) {
    }

    void KeyboardLuaBinding::create(lua_State* L, Keyboard* keyboard) {
        createKeyboardTable(L, keyboard);
        createKeyTable(L, keyboard);
    }

    void KeyboardLuaBinding::createKeyboardTable(lua_State* L, Keyboard* keyboard) {
        std::string typeName = "Keyboard";
        std::string metatableName = typeName + "__meta";

        // Allocate memory for the C++ object and push a userdata onto the Lua stack
        void* userdata = lua_newuserdata(L, sizeof(KeyboardLuaBinding));

        // Construct the C++ object in the allocated memory block
        new(userdata) KeyboardLuaBinding(keyboard);

        // Create a new metatable and push it onto the Lua stack
        luaL_newmetatable(L, metatableName.c_str());
        // Set the __gc metamethod of the metatable to KeyboardBinding::destroy
        {
            lua_pushstring(L, "__gc");
            lua_pushcfunction(L, KeyboardLuaBinding::destroy);
            lua_settable(L, -3);
        }
        // Set the __index metamethod of the metatable to KeyboardBinding::index
        {
            lua_pushstring(L, "__index");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, KeyboardLuaBinding::index, upvalueCount);
            lua_settable(L, -3);
        }
        // Set the newly created metatable as the metatable of the userdata
        lua_setmetatable(L, -2);

        // Create Lua object
        lua_newtable(L);

        // Associate the Lua table with the userdata (C++ object) as its user value
        lua_setuservalue(L, -2);

        // Associate the keyboard to the userdata as a light user value (C++ pointer)
        lua_pushlightuserdata(L, keyboard);
        lua_setuservalue(L, -2);

        // Create a global Lua variable and assign the userdata to it (C++ object)
        lua_setglobal(L, typeName.c_str());
    }

    void KeyboardLuaBinding::createKeyTable(lua_State* L, Keyboard* keyboard) {
        lua_newtable(L);
        for (std::pair<const std::string&, Key> keyPair : keyboard->getKeysByName()) {
            lua_pushstring(L, keyPair.first.c_str());
            lua_pushnumber(L, (uint16_t) keyPair.second);
            lua_settable(L, -3);
        }
        lua_setglobal(L, "Key");
    }

    LuaReturnValueCount KeyboardLuaBinding::destroy(lua_State* L) {
        // Lua stack
        // - [-1] userdata  Binding object
        auto* binding = (KeyboardLuaBinding*) lua_touserdata(L, -1);
        binding->~KeyboardLuaBinding();
        return 0;
    }

    LuaReturnValueCount KeyboardLuaBinding::index(lua_State* L) {
        // Lua stack
        // - [-1] string    Name of the index being accessed
        // - [-2] userdata  Binding object
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "isPressed") {
            lua_pushcfunction(L, KeyboardLuaBinding::isPressed);
            return 1;
        }
        return 0;
    }

    LuaReturnValueCount KeyboardLuaBinding::isPressed(lua_State* L) {
        // Lua stack
        // - [-1] number/string Function parameter: 'key'
        // - [-2] userdata      Binding object
        auto* binding = (KeyboardLuaBinding*) lua_touserdata(L, -2);
        bool pressed;
        if (lua_type(L, -1) == LUA_TSTRING) {
            std::string key = lua_tostring(L, -1);
            pressed = binding->keyboard->isPressed(key);
        } else {
            uint16_t key = lua_tonumber(L, -1);
            pressed = binding->keyboard->isPressed(key);
        }
        lua_pushboolean(L, pressed);
        return 1;
    }
}
