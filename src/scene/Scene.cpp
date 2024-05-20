#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "window/KeyEvent.h"
#include "graphics/Mesh.h"
#include "graphics/ViewProjection.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Blink {
    Scene::Scene(const SceneConfig& config) : config(config), useSceneCamera(true) {
        BL_ASSERT_THROW(!config.scene.empty());
        initializeEntities();
    }

    Scene::~Scene() {
        terminateEntities();
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
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::R) {
            config.luaEngine->compileLuaFiles();
            config.luaEngine->initializeEntityBindings(this);
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::T) {
            config.renderer->waitUntilIdle();
            terminateEntities();
            config.meshManager->resetDescriptors();
            config.luaEngine->compileLuaFiles();
            initializeEntities();
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Y) {
            config.sceneCamera->loggingEnabled = !config.sceneCamera->loggingEnabled;
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::I) {
            // Player entity logging
            // Replace this with forwarding events to lua scripts and handle key events there
            return;
        }
        config.sceneCamera->onEvent(event);
    }

    void Scene::update(double timestep) {
        config.luaEngine->updateEntities(this, timestep);

        for (const entt::entity entity : entityRegistry.view<TransformComponent, MeshComponent>()) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
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
                cameraComponent.view = glm::lookAt(transformComponent.position, transformComponent.position + transformComponent.forwardDirection, transformComponent.upDirection);
                cameraComponent.projection = config.sceneCamera->projection;
            }
        }
    }

    void Scene::render() {
        ViewProjection viewProjection{};
        if (!useSceneCamera) {
            const entt::entity& cameraEntity = entityRegistry.view<CameraComponent>().front();
            if (cameraEntity != entt::null) {
                const auto& cameraComponent = entityRegistry.get<CameraComponent>(cameraEntity);
                viewProjection.view = cameraComponent.view;
                viewProjection.projection = cameraComponent.projection;
            } else {
                useSceneCamera = true;
            }
        }
        if (useSceneCamera) {
            viewProjection.view = config.sceneCamera->getView();
            viewProjection.projection = config.sceneCamera->getProjection();
        }
        for (const entt::entity entity : entityRegistry.view<MeshComponent>()) {
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
            config.renderer->renderMesh(meshComponent.mesh, viewProjection);
        }
    }

    entt::entity Scene::createEntity() {
        entt::entity entity = entityRegistry.create();

        // Align directions with world space coordinate system (right-handed):
        // - Positive X --> Right
        // - Positive Y --> Up
        // - Positive Z --> Towards viewer / Out of screen
        glm::vec3 worldUpDirection = Y_AXIS;
        glm::vec3 forwardDirection = {Z_AXIS.x, Z_AXIS.y, -Z_AXIS.z};
        glm::vec3 rightDirection = glm::normalize(glm::cross(forwardDirection, worldUpDirection));
        glm::vec3 upDirection = glm::normalize(glm::cross(rightDirection, forwardDirection));

        // Align rotation with directions
        float yaw = 270.0f;

        TransformComponent transformComponent{};
        transformComponent.worldUpDirection = worldUpDirection;
        transformComponent.forwardDirection = forwardDirection;
        transformComponent.rightDirection = rightDirection;
        transformComponent.upDirection = upDirection;
        transformComponent.yaw = yaw;

        entityRegistry.emplace<TransformComponent>(entity, transformComponent);
        return entity;
    }

    void Scene::initializeEntities() {
        // Core bindings used by Lua scripts
        config.luaEngine->initializeCoreBindings(this);

        // Run Lua-script to create the entities for the scene and initialize them with components
        // Requires core bindings
        config.luaEngine->createEntities(config.scene);

        // Bindings to entities' associated Lua-script to be invoked each game update
        // Requires entities to have been created and initialized with components
        config.luaEngine->initializeEntityBindings(this);

        // Calculate transforms for entities in the scene
        // Requires entities to have been created for the scene (Lua script)
        for (const entt::entity entity : entityRegistry.view<TransformComponent>()) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            calculateTranslation(&transformComponent);
            calculateRotation(&transformComponent);
            calculateRotationOffsets(&transformComponent);
            calculateScale(&transformComponent);
        }

        // Load meshes for entities in the scene
        for (const entt::entity entity : entityRegistry.view<MeshComponent>()) {
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
            meshComponent.mesh = config.meshManager->getMesh(meshComponent.meshInfo);
        }
    }

    void Scene::terminateEntities() {
        entityRegistry.clear();
    }

    void Scene::calculateTranslation(TransformComponent* transformComponent) const {
        transformComponent->translation = glm::translate(glm::mat4(1.0f), transformComponent->position);
    }

    void Scene::calculateScale(TransformComponent* transformComponent) const {
        transformComponent->scale = glm::scale(glm::mat4(1.0f), transformComponent->size);
    }

    void Scene::calculateRotation(TransformComponent* transformComponent) const {
        glm::vec3& forward = transformComponent->forwardDirection;
        glm::vec3& right = transformComponent->rightDirection;
        glm::vec3& up = transformComponent->upDirection;
        glm::vec3& worldUp = transformComponent->worldUpDirection;

        float yawRadians = glm::radians(transformComponent->yaw);
        float pitchRadians = glm::radians(transformComponent->pitch);
        float rollRadians = glm::radians(transformComponent->roll);

        // Apply pitch and yaw rotation to calculate the new forward direction
        forward.x = clampToZero(cos(yawRadians) * cos(pitchRadians));
        forward.y = clampToZero(sin(pitchRadians));
        forward.z = clampToZero(sin(yawRadians) * cos(pitchRadians));
        forward = glm::normalize(forward);

        // Ensure orthogonality between forward, right, and up after pitch and yaw rotation
        right = glm::normalize(glm::cross(forward, worldUp));
        up = glm::normalize(glm::cross(right, forward));

        // Apply roll rotation to adjust right and up directions
        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), rollRadians, forward);
        right = glm::normalize(glm::vec3(rollMatrix * glm::vec4(right, 0.0f)));
        up = glm::normalize(glm::vec3(rollMatrix * glm::vec4(up, 0.0f)));

        // Ensure orthogonality between forward, right, and up after roll rotation
        up = glm::normalize(glm::cross(forward, right));
        forward = glm::normalize(glm::cross(right, up));

        // Calculate quaternions for yaw, pitch, and roll rotations
        glm::quat yawOrientation = glm::angleAxis(yawRadians, up);
        glm::quat pitchOrientation = glm::angleAxis(pitchRadians, right);
        glm::quat rollOrientation = glm::angleAxis(rollRadians, forward);

        // Combine quaternions to obtain the final orientation quaternion
        transformComponent->orientation = yawOrientation * pitchOrientation * rollOrientation;

        // Calculate the final rotation
        transformComponent->rotation = glm::toMat4(transformComponent->orientation * transformComponent->orientationOffset);
    }

    // --------------------------------------------------------------------------------------------------------------
    // Rotation offsets
    // --------------------------------------------------------------------------------------------------------------
    // Models may have been created with different coordinate systems. If the coordinate system used to create the
    // model is different to the one used by this app, the model's rotation will be inconsistent with the associated
    // transform when they are moved into world space.
    //
    // For example, a model may be facing the _positive_ Z-axis even though the transform says it's facing the
    // _negative_ Z-axis (i.e. it's facing the wrong way). In this case, we apply an extra offset of 180 degrees yaw
    // to make the model face the correct way.
    void Scene::calculateRotationOffsets(TransformComponent* transformComponent) const {
        float yawOffsetRadians = glm::radians(transformComponent->yawOffset);
        float pitchOffsetRadians = glm::radians(transformComponent->pitchOffset);
        float rollOffsetRadians = glm::radians(transformComponent->rollOffset);

        // Calculate quaternions for yaw, pitch, and roll rotation offsets
        glm::quat yawOffsetOrientation = glm::angleAxis(yawOffsetRadians, Y_AXIS);
        glm::quat pitchOffsetOrientation = glm::angleAxis(pitchOffsetRadians, X_AXIS);
        glm::quat rollOffsetOrientation = glm::angleAxis(rollOffsetRadians, Z_AXIS);

        // Combine quaternions to obtain the final orientation quaternion
        transformComponent->orientationOffset = yawOffsetOrientation * pitchOffsetOrientation * rollOffsetOrientation;
    }
}
