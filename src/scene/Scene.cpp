#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "CoordinateSystem.h"
#include "window/KeyEvent.h"
#include "graphics/Mesh.h"
#include "graphics/ViewProjection.h"

namespace Blink {
    Scene::Scene(const SceneConfig& config) : config(config) {
        BL_ASSERT_THROW(!config.scene.empty());
        initializeScene();
    }

    Scene::~Scene() {
        terminateScene();
    }

    void Scene::onEvent(Event& event) {
        // Use scene camera (Key: 1)
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Num_1) {
            activeCameraEntity = entt::null;
            return;
        }

        // Use a camera entity (Key: 2-8)
        if (event.type == EventType::KeyPressed) {
            auto key = event.as<KeyPressedEvent>().keyCode;
            uint32_t num2 = (uint32_t) Key::Num_2;
            uint32_t num8 = (uint32_t) Key::Num_8;
            if (key >= num2 && key < num8) {
                uint32_t cameraIndex = key - num2;
                uint32_t i = 0;
                for (const entt::entity entity : entityRegistry.view<CameraComponent>()) {
                    if (cameraIndex == i) {
                        activeCameraEntity = entity;
                        break;
                    }
                    i++;
                }
                return;
            }
        }

        // Toggle scene camera debug logging
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Num_9) {
            config.sceneCamera->loggingEnabled = !config.sceneCamera->loggingEnabled;
            return;
        }

        // Reset the scene camera
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Num_0) {
            configureSceneCameraWithDefaultSettings();
            config.luaEngine->configureSceneCamera(config.scene);
            config.sceneCamera->calculateProjection();
            return;
        }

        // Recompile Lua scripts while the scene is running (hot reload)
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::R) {
            config.luaEngine->compileLuaFiles();
            config.luaEngine->initializeCoreBindings(this);
            for (const entt::entity entity : entityRegistry.view<LuaComponent>()) {
                auto& luaComponent = entityRegistry.get<LuaComponent>(entity);
                auto& tagComponent = entityRegistry.get<TagComponent>(entity);
                config.luaEngine->initializeEntityBinding(entity, luaComponent, tagComponent);
            }
            return;
        }

        // Unload, recompile and load entire scene again (cold reload)
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::T) {
            terminateScene();
            config.luaEngine->compileLuaFiles();
            initializeScene();
            return;
        }

        // Update scene camera and all camera entites when the window resizes
        if (event.type == EventType::WindowResize) {
            config.sceneCamera->onResize();
            for (const entt::entity entity : entityRegistry.view<CameraComponent>()) {
                auto& cameraComponent = entityRegistry.get<CameraComponent>(entity);
                cameraComponent.aspectRatio = config.sceneCamera->aspectRatio;
            }
        }
    }

    //
    // Update sequence:
    // 1. Run Lua-scripts
    // 2. Calculate transforms (all entities must have a transform to exist in the world)
    // 3. Calculate model matrices for rendering
    // 4. Calculate camera view (either the active camera entity or the scene camera)
    //
    // NOTE:
    // All non-camera entities must be updated _before_ camera entities.
    // This is because cameras often need to track other entities, so the transforms of the entities being tracked must
    // always be up-to-date and correct when used by the camera(s).
    //
    void Scene::update(double timestep) {
        // Run Lua-scripts for all non-camera entites
        for (const entt::entity entity : entityRegistry.view<LuaComponent>(entt::exclude<CameraComponent>)) {
            auto& luaComponent = entityRegistry.get<LuaComponent>(entity);
            auto& tagComponent = entityRegistry.get<TagComponent>(entity);
            config.luaEngine->updateEntity(entity, luaComponent, tagComponent, timestep);
        }

        // Calculate transforms for all non-camera entities
        for (const entt::entity entity : entityRegistry.view<TransformComponent>(entt::exclude<CameraComponent>)) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            calculateTranslation(&transformComponent);
            calculateRotation(&transformComponent);
            calculateScale(&transformComponent);
        }

        // Run Lua-scripts for all camera entites
        for (const entt::entity entity : entityRegistry.view<LuaComponent, CameraComponent>()) {
            auto& luaComponent = entityRegistry.get<LuaComponent>(entity);
            auto& tagComponent = entityRegistry.get<TagComponent>(entity);
            config.luaEngine->updateEntity(entity, luaComponent, tagComponent, timestep);
        }

        // Calculate transforms for all camera entities
        for (const entt::entity entity : entityRegistry.view<TransformComponent, CameraComponent>()) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            calculateTranslation(&transformComponent);
            calculateCameraRotation(&transformComponent);
            calculateScale(&transformComponent);
        }

        // Calculate model matrices for all entities (both cameras and non-cameras)
        for (const entt::entity entity : entityRegistry.view<TransformComponent, MeshComponent>()) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
            meshComponent.mesh->model = transformComponent.translation * transformComponent.rotation * transformComponent.scale;
        }

        // Calculate camera view-projection
        if (activeCameraEntity != entt::null) {
            auto& cameraComponent = entityRegistry.get<CameraComponent>(activeCameraEntity);
            auto& transformComponent = entityRegistry.get<TransformComponent>(activeCameraEntity);
            calculateCameraView(&cameraComponent, &transformComponent);
            calculateCameraProjection(&cameraComponent);
        } else {
            config.sceneCamera->update(timestep);
        }
    }

    void Scene::render() {
        // Pass the view and projection matrices of the camera to the renderer
        ViewProjection viewProjection{};
        if (activeCameraEntity != entt::null) {
            const auto& cameraComponent = entityRegistry.get<CameraComponent>(activeCameraEntity);
            viewProjection.view = cameraComponent.view;
            viewProjection.projection = cameraComponent.projection;
        } else {
            viewProjection.view = config.sceneCamera->view;
            viewProjection.projection = config.sceneCamera->projection;
        }
        config.renderer->setViewProjection(viewProjection);

        // Render skybox
        config.renderer->renderSkybox(config.skybox);

        // Render all meshes in the scene
        for (const entt::entity entity : entityRegistry.view<MeshComponent>()) {
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
            bool isActiveCameraEntity = activeCameraEntity != entt::null && entity == activeCameraEntity;
            if (isActiveCameraEntity) {
                continue; // Don't draw the mesh of the currently active camera entity
            }
            config.renderer->renderMesh(meshComponent.mesh);
        }
    }

    entt::entity Scene::createEntityWithDefaultComponents() {
        entt::entity entity = entityRegistry.create();

        // All entities must have a tag for easier identification and debugging
        TagComponent tagComponent{};
        tagComponent.tag = "Entity " + std::to_string((uint32_t) entity);
        entityRegistry.emplace<TagComponent>(entity, tagComponent);

        // All entities must have a transform to be able to exist in the world
        TransformComponent transformComponent{};
        transformComponent.translation = glm::mat4(1.0f);
        transformComponent.rotation = glm::mat4(1.0f);
        transformComponent.scale = glm::mat4(1.0f);
        transformComponent.position = glm::vec3(0.0f, 0.0f, 0.0f);
        transformComponent.size = glm::vec3(1.0f, 1.0f, 1.0f);
        transformComponent.forwardDirection = WORLD_FORWARD_DIRECTION;
        transformComponent.rightDirection = WORLD_RIGHT_DIRECTION;
        transformComponent.upDirection = WORLD_UP_DIRECTION;
        transformComponent.worldUpDirection = WORLD_UP_DIRECTION;
        transformComponent.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        transformComponent.yaw = 0.0f;
        transformComponent.pitch = 0.0f;
        transformComponent.roll = 0.0f;
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
        for (const entt::entity entity : entityRegistry.view<LuaComponent>()) {
            auto& luaComponent = entityRegistry.get<LuaComponent>(entity);
            auto& tagComponent = entityRegistry.get<TagComponent>(entity);
            config.luaEngine->initializeEntityBinding(entity, luaComponent, tagComponent);
        }

        // Calculate transforms for all non-camera entities
        // REQUIRES entities to have been created
        for (const entt::entity entity : entityRegistry.view<TransformComponent>(entt::exclude<CameraComponent>)) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            calculateTranslation(&transformComponent);
            calculateRotation(&transformComponent);
            calculateScale(&transformComponent);
        }

        // Calculate transforms and view-projection for all camera entities
        // REQUIRES entities to have been created
        for (const entt::entity entity : entityRegistry.view<TransformComponent, CameraComponent>()) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            auto& cameraComponent = entityRegistry.get<CameraComponent>(entity);
            calculateTranslation(&transformComponent);
            calculateCameraRotation(&transformComponent);
            calculateScale(&transformComponent);
            calculateCameraView(&cameraComponent, &transformComponent);
            calculateCameraProjection(&cameraComponent);
        }

        // Load meshes for entities in the scene
        // REQUIRES entities to have been created
        for (const entt::entity entity : entityRegistry.view<MeshComponent>()) {
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
            meshComponent.mesh = config.meshManager->getMesh(meshComponent.meshInfo);
        }
    }

    void Scene::terminateScene() {
        // Wait until it's safe to destroy the resources (descriptors) used by the meshes in the current scene
        config.renderer->waitUntilIdle();

        // Unload scene
        activeCameraEntity = entt::null;
        entityRegistry.clear();
        config.luaEngine->resetState();
        config.meshManager->resetDescriptors();
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

        // Keep direction vectors consistent with the orientation
        transformComponent->rightDirection = glm::normalize(transformComponent->orientation * WORLD_RIGHT_DIRECTION);
        transformComponent->upDirection = glm::normalize(transformComponent->orientation * WORLD_UP_DIRECTION);
        transformComponent->forwardDirection = glm::normalize(transformComponent->orientation * WORLD_FORWARD_DIRECTION);

        transformComponent->rotation = glm::toMat4(transformComponent->orientation);
    }

    void Scene::calculateScale(TransformComponent* transformComponent) const {
        transformComponent->scale = glm::scale(glm::mat4(1.0f), transformComponent->size);
    }

    void Scene::calculateCameraRotation(TransformComponent* transformComponent) const {
        // Unlike non-camera entities, the orientation is calculated in the camera's Lua-script
        glm::quat orientation = transformComponent->orientation;

        // Keep direction vectors consistent with the orientation
        transformComponent->rightDirection = glm::normalize(orientation * WORLD_RIGHT_DIRECTION);
        transformComponent->upDirection = glm::normalize(orientation * WORLD_UP_DIRECTION);
        transformComponent->forwardDirection = glm::normalize(orientation * WORLD_FORWARD_DIRECTION);

        // Use the _inverse_ orientation to make sure the mesh will have the correct orientation relative to the world
        transformComponent->rotation = glm::toMat4(glm::inverse(orientation));
    }

    void Scene::calculateCameraView(CameraComponent* cameraComponent, TransformComponent* transformComponent) const {
        // Convert the orientation quaternion directly into a rotation matrix
        glm::mat4 viewRotation = glm::toMat4(transformComponent->orientation);

        // Translate the camera to the negative of its position to move the world in the opposite direction
        glm::mat4 viewTranslation = glm::translate(glm::mat4(1.0), -transformComponent->position);

        // Combine the rotation and translation to form the view matrix
        // Don't want to use the glm::lookAt function to avoid issues with directional vectors and gimbal locking
        cameraComponent->view = viewRotation * viewTranslation;
    }

    void Scene::calculateCameraProjection(CameraComponent* cameraComponent) const {
        cameraComponent->projection = glm::perspective(
            cameraComponent->fieldOfView,
            cameraComponent->aspectRatio,
            cameraComponent->nearClip,
            cameraComponent->farClip
        );
    }
}
