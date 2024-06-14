#include "SkyboxLuaBinding.h"

namespace Blink {
    SkyboxLuaBinding::SkyboxLuaBinding(Scene* scene) : scene(scene) {
    }

    void SkyboxLuaBinding::initialize(lua_State* L, Scene* scene) {
        std::string typeName = "Skybox";
        std::string metatableName = typeName + "__meta";

        // Allocate memory for the C++ object and push a userdata onto the Lua stack
        void* userdata = lua_newuserdata(L, sizeof(SkyboxLuaBinding));

        // Construct the C++ object in the allocated memory block
        new(userdata) SkyboxLuaBinding(scene);

        // Create a new metatable and push it onto the Lua stack
        luaL_newmetatable(L, metatableName.c_str());

        // Set the __gc metamethod of the metatable to binding destroy function
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, SkyboxLuaBinding::destroy);
        lua_settable(L, -3);

        // Set the __index metamethod of the metatable to binding index function
        lua_pushstring(L, "__index");
        constexpr int upvalueCount = 0;
        lua_pushcclosure(L, SkyboxLuaBinding::index, upvalueCount);
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
    // - [-1] userdata  Binding
    int SkyboxLuaBinding::destroy(lua_State* L) {
        auto* binding = (SkyboxLuaBinding*) lua_touserdata(L, -1);
        binding->~SkyboxLuaBinding();
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] userdata  Binding
    int SkyboxLuaBinding::index(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "setSkybox") {
            lua_pushcfunction(L, SkyboxLuaBinding::setSkybox);
            return 1;
        }
        BL_LOG_WARN("Could not resolve index [{}]", indexName);
        return 0;
    }

    // Lua stack
    // - [-1] table     Skybox image file paths list
    // - [-2] userdata  Binding
    int SkyboxLuaBinding::setSkybox(lua_State* L) {
        std::vector<std::string> skyboxImagePaths;
        for (int i = 0; i < Skybox::FACE_COUNT; ++i) {
            lua_geti(L, -1, i + 1);
            BL_ASSERT(lua_type(L, -1) != LUA_TNIL);
            skyboxImagePaths.emplace_back(lua_tostring(L, -1));
            lua_pop(L, 1);
        }
        auto binding = (SkyboxLuaBinding*) lua_touserdata(L, -2);
        binding->scene->setSkybox(skyboxImagePaths);
        return 0;
    }
}
