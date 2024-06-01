#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "CoordinateSystem.h"
#include "window/KeyEvent.h"
#include "graphics/Mesh.h"
#include "graphics/ViewProjection.h"

namespace Blink {
    Scene::Scene(const SceneConfig& config) : config(config), useSceneCamera(true) {
        BL_ASSERT_THROW(!config.scene.empty());
        initializeScene();
    }

    Scene::~Scene() {
        terminateScene();
    }

    void Scene::onEvent(Event& event) {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Num_1) {
            useSceneCamera = true;
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Num_2) {
            useSceneCamera = false;
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Num_0) {
            resetSceneCamera();
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Y) {
            config.sceneCamera->loggingEnabled = !config.sceneCamera->loggingEnabled;
            return;
        }
        // Recompile Lua scripts while the scene is running (hot reload)
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::R) {
            config.luaEngine->reloadLuaScripts(this);
            return;
        }
        // Unload, recompile and load entire scene again (cold reload)
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::T) {
            terminateScene();
            config.luaEngine->reloadLuaScripts(this);
            initializeScene();
            return;
        }
        config.sceneCamera->onEvent(event);
    }

    void Scene::update(double timestep) {
        config.luaEngine->updateEntities(this, timestep);

        for (const entt::entity entity : entityRegistry.view<TransformComponent, MeshComponent>()) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);

            bool isCamera = entityRegistry.try_get<CameraComponent>(entity) != nullptr;
            if (false) {
                glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(transformComponent.orientation));
                transformComponent.pitch = -eulerAngles.x;
                transformComponent.yaw = -eulerAngles.y;
                transformComponent.roll = -eulerAngles.z;

                transformComponent.rightDirection = glm::normalize(transformComponent.orientation * WORLD_RIGHT_DIRECTION);
                transformComponent.upDirection = glm::normalize(transformComponent.orientation * WORLD_UP_DIRECTION);
                transformComponent.forwardDirection = glm::normalize(transformComponent.orientation * WORLD_FORWARD_DIRECTION);

                transformComponent.rotation = glm::toMat4(transformComponent.orientation);
            }

            calculateTranslation(&transformComponent);
            calculateRotation(&transformComponent);
            calculateScale(&transformComponent);


            meshComponent.mesh->model = transformComponent.translation * transformComponent.rotation * transformComponent.scale;
        }

        if (useSceneCamera) {
            config.sceneCamera->update(timestep);
        } else {
            for (const entt::entity entity : entityRegistry.view<TransformComponent, CameraComponent>()) {
                auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
                auto& cameraComponent = entityRegistry.get<CameraComponent>(entity);
                // cameraComponent.view = glm::lookAt(
                //     transformComponent.position,
                //     transformComponent.position + transformComponent.forwardDirection,
                //     transformComponent.upDirection
                // );
                cameraComponent.projection = glm::perspective(
                    cameraComponent.fieldOfView,
                    cameraComponent.aspectRatio,
                    cameraComponent.nearClip,
                    cameraComponent.farClip
                );
            }
        }
    }

    void Scene::render() {
        entt::entity cameraEntity = entityRegistry.view<CameraComponent>().front();
        bool useSceneCamera = this->useSceneCamera || cameraEntity == entt::null;

        ViewProjection viewProjection{};
        if (useSceneCamera) {
            viewProjection.view = config.sceneCamera->view;
            viewProjection.projection = config.sceneCamera->projection;
        } else {
            const auto& cameraComponent = entityRegistry.get<CameraComponent>(cameraEntity);
            viewProjection.view = cameraComponent.view;
            viewProjection.projection = cameraComponent.projection;
        }
        config.renderer->setViewProjection(viewProjection);

        for (const entt::entity entity : entityRegistry.view<MeshComponent>()) {
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
            auto* cameraComponent = entityRegistry.try_get<CameraComponent>(entity);
            if (cameraComponent != nullptr && !useSceneCamera) {
                continue; // Don't draw the mesh of the camera that's in use
            }
            config.renderer->renderMesh(meshComponent.mesh);
        }
    }

    entt::entity Scene::createEntityWithDefaultComponents() {
        entt::entity entity = entityRegistry.create();

        TagComponent tagComponent{};
        tagComponent.tag = "Entity " + std::to_string((uint32_t) entity);
        entityRegistry.emplace<TagComponent>(entity, tagComponent);

        TransformComponent transformComponent{};
        transformComponent.forwardDirection = WORLD_FORWARD_DIRECTION;
        transformComponent.rightDirection = WORLD_RIGHT_DIRECTION;
        transformComponent.upDirection = WORLD_UP_DIRECTION;
        transformComponent.worldUpDirection = WORLD_UP_DIRECTION;
        entityRegistry.emplace<TransformComponent>(entity, transformComponent);

        return entity;
    }

    void Scene::initializeScene() {
        // Core bindings used by Lua scripts
        config.luaEngine->initializeCoreBindings(this);

        // Run Lua-script to configure scene camera with scene-specific settings
        // REQUIRES core bindings
        configureSceneCameraWithDefaultSettings();
        config.luaEngine->configureSceneCamera(config.scene);

        // Run Lua-script to create the entities for the scene and initialize them with components
        // REQUIRES core bindings and scene camera configuration
        config.luaEngine->createEntities(config.scene);

        // Configure bindings to entities' associated Lua-script to be invoked each game update
        // REQUIRES entities to have been created
        config.luaEngine->initializeEntityBindings(this);

        // Calculate transforms for entities in the scene
        // REQUIRES entities to have been created
        for (const entt::entity entity : entityRegistry.view<TransformComponent>()) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            calculateTranslation(&transformComponent);
            calculateRotation(&transformComponent);
            calculateScale(&transformComponent);
        }

        // Load meshes for entities in the scene
        // Requires entities to have been created
        for (const entt::entity entity : entityRegistry.view<MeshComponent>()) {
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
            meshComponent.mesh = config.meshManager->getMesh(meshComponent.meshInfo);
        }
    }

    void Scene::terminateScene() {
        // Wait until it's safe to destroy the resources (descriptors) used by the meshes in the current scene
        config.renderer->waitUntilIdle();

        // Unload scene
        entityRegistry.clear();
        config.luaEngine->resetState();
        config.meshManager->resetDescriptors();
    }

    void Scene::resetSceneCamera() const {
        configureSceneCameraWithDefaultSettings();
        config.luaEngine->configureSceneCamera(config.scene);
    }

    void Scene::configureSceneCameraWithDefaultSettings() const {
        config.sceneCamera->position = {0.0f, 0.0f, 0.0f};
        config.sceneCamera->moveSpeed = 1.0f;
        config.sceneCamera->rotationSpeed = 1.0f;
        config.sceneCamera->fieldOfView = 45.0f;
        config.sceneCamera->nearClip = 0.1f;
        config.sceneCamera->farClip = 100.0f;
        config.sceneCamera->yaw = 0.0f;
        config.sceneCamera->pitch = 0.0f;
        config.sceneCamera->roll = 0.0f;
        config.sceneCamera->orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        config.sceneCamera->forwardDirection = WORLD_FORWARD_DIRECTION;
        config.sceneCamera->rightDirection = WORLD_RIGHT_DIRECTION;
        config.sceneCamera->upDirection = WORLD_UP_DIRECTION;
        config.sceneCamera->worldUpDirection = WORLD_UP_DIRECTION;
    }

    void Scene::calculateTranslation(TransformComponent* transformComponent) const {
        transformComponent->translation = glm::translate(glm::mat4(1.0f), transformComponent->position);
    }

    void Scene::calculateRotation(TransformComponent* transformComponent) const {
        float yawRadians = glm::radians(transformComponent->yaw);
        float pitchRadians = glm::radians(transformComponent->pitch);
        float rollRadians = glm::radians(transformComponent->roll);

        glm::quat yawRotation = glm::normalize(glm::angleAxis(yawRadians, POSITIVE_Y_AXIS));
        glm::quat pitchRotation = glm::normalize(glm::angleAxis(pitchRadians, POSITIVE_X_AXIS));
        glm::quat rollRotation = glm::normalize(glm::angleAxis(rollRadians, POSITIVE_Z_AXIS));

        // Multiplication order is crucial.
        // Changing the order changes the final orientation, similar to the behavior of matrix multiplication.
        transformComponent->orientation = glm::normalize(yawRotation * pitchRotation * rollRotation);

        transformComponent->rightDirection = glm::normalize(transformComponent->orientation * WORLD_RIGHT_DIRECTION);
        transformComponent->upDirection = glm::normalize(transformComponent->orientation * WORLD_UP_DIRECTION);
        transformComponent->forwardDirection = glm::normalize(transformComponent->orientation * WORLD_FORWARD_DIRECTION);

        transformComponent->rotation = glm::toMat4(transformComponent->orientation);
    }

    void Scene::calculateScale(TransformComponent* transformComponent) const {
        transformComponent->scale = glm::scale(glm::mat4(1.0f), transformComponent->size);
    }
}
