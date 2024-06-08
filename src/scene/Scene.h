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
        SceneConfig config;
        entt::registry entityRegistry;
        entt::entity activeCameraEntity = entt::null;

    public:
        explicit Scene(const SceneConfig& config);

        ~Scene();

        void onEvent(Event& event);

        void update(double timestep);

        void render();

        entt::entity createEntityWithDefaultComponents();

    private:
        void initializeScene();

        void terminateScene();

        void configureSceneCameraWithDefaultSettings() const;

        void calculateTranslation(TransformComponent* transformComponent) const;

        void calculateRotation(TransformComponent* transformComponent) const;

        void calculateCameraRotation(TransformComponent* transformComponent) const;

        void calculateScale(TransformComponent* transformComponent) const;

        void calculateCameraView(CameraComponent* cameraComponent, TransformComponent* transformComponent) const;

        void calculateCameraProjection(CameraComponent* cameraComponent) const;

        bool isCameraEntity(entt::entity entity) const;
    };
}
