#pragma once

#include "window/Keyboard.h"
#include "graphics/Renderer.h"
#include "lua/LuaEngine.h"
#include "scene/SceneCamera.h"
#include "scene/Components.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace Blink {
    struct SceneConfig {
        std::string scene;
        Keyboard* keyboard = nullptr;
        MeshManager* meshManager = nullptr;
        Renderer* renderer = nullptr;
        LuaEngine* luaEngine = nullptr;
        SceneCamera* sceneCamera = nullptr;
    };

    class Scene {
        friend class LuaEngine;
        friend class EntityLuaBinding;

    private:
        static const glm::vec3 X_AXIS;
        static const glm::vec3 Y_AXIS;
        static const glm::vec3 Z_AXIS;

    private:
        SceneConfig config;
        entt::registry entityRegistry;
        bool useSceneCamera = false;

    public:
        explicit Scene(const SceneConfig& config);

        ~Scene();

        void onEvent(Event& event);

        void update(double timestep);

        void render();

    private:
        void updateEntityDirections();

        void updateDirections(TransformComponent* transformComponent) const;

        void loadEntityMeshes();

        entt::entity createEntity();

        void initializeCamera() const;

        void createEntities();

        void destroyEntities();
    };
}
