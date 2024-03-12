#include "pch.h"
#include "EntityLuaBinding.h"
#include "LuaEngine.h"
#include "printLua.h"

namespace Blink {
    EntityLuaBinding::EntityLuaBinding(
        const EntityLuaBindingConfig& config
    ) : LuaBinding(config.L, config.tableName),
        registry(config.registry),
        entity(config.entity)
    {
    }

    void EntityLuaBinding::createType(lua_State* L, const std::string& typeName, entt::registry* registry) {
        std::string metatableName = typeName + "__meta";
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setglobal(L, typeName.c_str());
        {
            lua_pushstring(L, typeName.c_str());
            lua_pushlightuserdata(L, registry);
            constexpr int upvalueCount = 2;
            lua_pushcclosure(L, EntityLuaBinding::createInstance, upvalueCount);
            lua_setfield(L, -2, "new");
        }
        luaL_newmetatable(L, metatableName.c_str());
        {
            lua_pushstring(L, "__gc");
            lua_pushcfunction(L, EntityLuaBinding::destroy);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__index");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, EntityLuaBinding::index, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__newindex");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, EntityLuaBinding::newIndex, upvalueCount);
            lua_settable(L, -3);
        }
        // lua_setmetatable(L, -2);
        // printLua(L, "NOTICE ME");
    }

    int EntityLuaBinding::createInstance(lua_State* L) {
        BL_LOG_INFO("createInstance");
        // std::string typeName = lua_tostring(L, lua_upvalueindex(1));
        // std::string metatableName = typeName + "__meta";
        //
        // std::string entityId = lua_tostring(L, 1);
        // auto* registry = static_cast<entt::registry*>(lua_touserdata(L, lua_upvalueindex(2)));
        // Entity& entity = scene->entities[entityId];
        //
        // void* userdata = lua_newuserdata(L, sizeof(EntityBinding));
        // new(userdata) EntityBinding(&entity);
        // int userdataIndex = lua_gettop(L);
        //
        // luaL_getmetatable(L, metatableName.c_str());
        // lua_setmetatable(L, userdataIndex);
        //
        // lua_newtable(L);
        // lua_setuservalue(L, userdataIndex);
        //
        // lua_getglobal(L, typeName.c_str());
        // const char* onCreateFn = "onCreate";
        // lua_getfield(L, -1, onCreateFn);
        // lua_setfield(L, -3, onCreateFn);
        // const char* onUpdateFn = "onUpdate";
        // lua_getfield(L, -1, onUpdateFn);
        // lua_setfield(L, -3, onUpdateFn);
        // const char* onDestroyFn = "onDestroy";
        // lua_getfield(L, -1, onDestroyFn);
        // lua_setfield(L, -3, onDestroyFn);
        // lua_pop(L, 1);

        return 1;
    }

    int EntityLuaBinding::destroy(lua_State* L) {
        BL_LOG_INFO("destroy");
        // auto* binding = static_cast<EntityBinding*>(lua_touserdata(L, -1));
        // binding->~EntityBinding();
        return 0;
    }

    int EntityLuaBinding::index(lua_State* L) {
        BL_LOG_INFO("index");
        constexpr int bottomOfLuaStackIndex = 1;
        int userdataIndex = bottomOfLuaStackIndex;
        int keyIndex = userdataIndex + 1;
        std::string key = lua_tostring(L, keyIndex);
        BL_LOG_INFO("index [{}]", key);

        // bool indexFound = false;
        // if (key == "whoami") {
        //     BL_LOG_INFO("YES HELLO IT IS I");
        //     // auto* binding = (EntityLuaBinding*) lua_touserdata(L, userdataIndex);
        //     // lua_pushstring(L, binding->getId().c_str());
        //     indexFound = true;
        // }
        // if (key == "type") {
        //     auto* binding = (EntityBinding*) lua_touserdata(L, userdataIndex);
        //     auto* scriptComponent = (ScriptComponent*) binding->getComponent("scriptComponent");
        //     lua_pushstring(L, scriptComponent->type.c_str());
        //     indexFound = true;
        // }
        // if (key == "transform") {
        //     auto* binding = (EntityBinding*) lua_touserdata(L, userdataIndex);
        //     auto* transformComponent = (TransformComponent*) binding->getComponent("transformComponent");
        //     lua_newtable(L);
        //     lua_pushnumber(L, transformComponent->x);
        //     lua_setfield(L, -2, "x");
        //     lua_pushnumber(L, transformComponent->y);
        //     lua_setfield(L, -2, "y");
        //     lua_pushnumber(L, transformComponent->z);
        //     lua_setfield(L, -2, "z");
        //     indexFound = true;
        // }
        // if (key == "hasComponent") {
        //     auto function = [](lua_State* L) -> int {
        //         std::string componentType = lua_tostring(L, -1);
        //         auto* binding = (EntityBinding*) lua_touserdata(L, lua_upvalueindex(1));
        //         lua_pushboolean(L, binding->hasComponent(componentType));
        //         return 1;
        //     };
        //     auto* binding = (EntityBinding*) lua_touserdata(L, userdataIndex);
        //     lua_pushlightuserdata(L, binding);
        //     constexpr int upvalueCount = 1;
        //     lua_pushcclosure(L, function, upvalueCount);
        //     indexFound = true;
        // }
        // if (!indexFound) {
        //     // Key does not exist on C++ class, attempt to get it from lua table
        //     lua_getuservalue(L, userdataIndex);
        //     lua_pushvalue(L, keyIndex);
        //     lua_gettable(L, -2);
        // }
        return 1;
    }

    int EntityLuaBinding::newIndex(lua_State* L) {
        BL_LOG_INFO("newIndex");
        // constexpr int bottomOfLuaStackIndex = 1;
        // int userdataIndex = bottomOfLuaStackIndex;
        // int keyIndex = userdataIndex + 1;
        // int valueIndex = keyIndex + 1;
        // std::string key = lua_tostring(L, keyIndex);
        //
        // bool indexFound = false;
        // if (key == "entityId") {
        //     const char* value = lua_tostring(L, valueIndex);
        //     auto* binding = (EntityBinding*) lua_touserdata(L, userdataIndex);
        //     binding->setId(value);
        //     indexFound = true;
        // }
        // if (!indexFound) {
        //     // Key does not exist on C++ class, attempt to set key=value on lua table
        //     lua_getuservalue(L, userdataIndex);
        //     lua_pushvalue(L, keyIndex);
        //     lua_pushvalue(L, valueIndex);
        //     lua_settable(L, -3);
        // }
        return 1;
    }
}
