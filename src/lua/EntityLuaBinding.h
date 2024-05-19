#pragma once

#include <entt/entt.hpp>
#include <lua.hpp>

namespace Blink {

    // Forward declaration
    class Scene;

    class EntityLuaBinding {
    private:
        Scene* scene;

    private:
        explicit EntityLuaBinding(Scene* scene);

        ~EntityLuaBinding() = default;

    public:
        static void initialize(lua_State* L, Scene* scene);

    private:
        static int destroy(lua_State* L);

        static int index(lua_State* L);

        static int createEntity(lua_State* L);

        static int setMeshComponent(lua_State* L);

        static int setLuaComponent(lua_State* L);

        static int getTagComponent(lua_State* L);

        static int setTagComponent(lua_State* L);

        static int getTransformComponent(lua_State* L);

        static int setTransformComponent(lua_State* L);

        static int setCameraComponent(lua_State* L);

        static int getPosition(lua_State* L);

        static int setPosition(lua_State* L);

        static int getIdByTag(lua_State* L);
    };
}
