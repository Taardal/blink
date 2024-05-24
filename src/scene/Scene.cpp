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

        // Recompile Lua scripts while the scene is running (hot reload)
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::R) {
            config.luaEngine->compileLuaFiles();
            config.luaEngine->initializeEntityBindings(this);
            return;
        }

        // Unload, recompile and load entire scene again (cold reload)
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::T) {
            // Wait until it's safe to destroy the resources (descriptors) used by the meshes in the current scene
            config.renderer->waitUntilIdle();

            // Unload scene
            terminateScene();
            config.meshManager->resetDescriptors();
            config.luaEngine->resetState();

            // Recompile scene & entity scripts
            config.luaEngine->compileLuaFiles();

            // Load scene
            initializeScene();
            return;
        }

        // Debug logging
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

        for (const entt::entity entity : entityRegistry.view<TransformComponent, MeshComponent, TagComponent>()) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
            auto& tagComponent = entityRegistry.get<TagComponent>(entity);

            calculateTranslation(&transformComponent);
            //calculateRotation(&transformComponent);
            //calculateScale(&transformComponent);

            // --------------------------------------------------------------------------------------------------------------

            if (tagComponent.tag == "Player") {
                glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

#define FOOBAR
#ifdef FOO
                glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
                glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
                glm::vec3 up = glm::normalize(glm::cross(right, forward));

                // glm::vec3 forward(0.0f, 0.0f, 0.0f);
                // glm::vec3 right(0.0f, 0.0f, 0.0f);
                // glm::vec3 up(0.0f, 1.0f, 0.0f);

                glm::mat3 rotationMatrix;
                rotationMatrix[0] = right;
                rotationMatrix[1] = up;
                rotationMatrix[2] = forward;

                orientation = glm::toQuat(rotationMatrix);

                transformComponent.forwardDirection = forward;
                transformComponent.rightDirection = right;
                transformComponent.upDirection = up;
#else
                float yaw = transformComponent.yaw + 1;
                float pitch = transformComponent.pitch + 1;
                float roll = 0;

                float yawRadians = glm::radians(yaw);
                float pitchRadians = glm::radians(pitch);
                float rollRadians = glm::radians(roll);

                transformComponent.yaw = yaw;
                transformComponent.pitch = pitch;
                transformComponent.roll = roll;

                glm::quat yawRotation = glm::angleAxis(yawRadians, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::quat pitchRotation = glm::angleAxis(pitchRadians, glm::vec3(1.0f, 0.0f, 0.0f));
                glm::quat rollRotation = glm::angleAxis(rollRadians, glm::vec3(0.0f, 0.0f, 1.0f));

                orientation = (yawRotation * pitchRotation * rollRotation) * orientation;

                transformComponent.forwardDirection = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
                transformComponent.rightDirection = orientation * glm::vec3(1.0f, 0.0f, 0.0f);
                transformComponent.upDirection = orientation * glm::vec3(0.0f, 1.0f, 0.0f);
#endif

                glm::mat4 rotation = glm::toMat4(orientation);

                transformComponent.orientation = orientation;
                //transformComponent.rotation = rotation;
            }

            // --------------------------------------------------------------------------------------------------------------

            meshComponent.mesh->model = transformComponent.translation * transformComponent.rotation * transformComponent.scale;
        }

        if (useSceneCamera) {
            config.sceneCamera->update(timestep);
        } else {
            for (const entt::entity entity : entityRegistry.view<TransformComponent, CameraComponent>()) {
                auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
                auto& cameraComponent = entityRegistry.get<CameraComponent>(entity);
                cameraComponent.view = glm::lookAt(
                    transformComponent.position,
                    transformComponent.position + transformComponent.forwardDirection,
                    transformComponent.upDirection
                );
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
            config.renderer->renderMesh(meshComponent.mesh);
        }
    }

    entt::entity Scene::createEntity() {
        entt::entity entity = entityRegistry.create();

        TagComponent tagComponent{};
        tagComponent.tag = "Entity " + std::to_string((uint32_t) entity);
        entityRegistry.emplace<TagComponent>(entity, tagComponent);

        // Align directions with world space coordinate system (right-handed):
        // - Positive X --> Right
        // - Positive Y --> Up
        // - Positive Z --> Towards viewer / Out of screen
        glm::vec3 worldUp = POSITIVE_Y_AXIS;
        glm::vec3 forward = NEGATIVE_Z_AXIS;
        glm::vec3 right = POSITIVE_X_AXIS;
        glm::vec3 up = POSITIVE_Y_AXIS;

        glm::mat3 rotationMatrix;
        rotationMatrix[0] = right;
        rotationMatrix[1] = up;
        rotationMatrix[2] = forward;

        glm::quat orientation = glm::toQuat(rotationMatrix);
        glm::mat4 rotation = glm::toMat4(orientation);

        glm::vec3 eulerAngles = glm::eulerAngles(orientation);
        eulerAngles = glm::degrees(eulerAngles);

        TransformComponent transformComponent{};
        transformComponent.worldUpDirection = worldUp;
        transformComponent.forwardDirection = forward;
        transformComponent.rightDirection = right;
        transformComponent.upDirection = up;
        transformComponent.orientation = orientation;
        transformComponent.rotation = rotation;
        transformComponent.yaw = eulerAngles.y;
        transformComponent.pitch = eulerAngles.x;
        transformComponent.roll = eulerAngles.z;
        entityRegistry.emplace<TransformComponent>(entity, transformComponent);

        return entity;
    }

    void Scene::initializeScene() {
        // Core bindings used by Lua scripts
        config.luaEngine->initializeCoreBindings(this);

        // Configure scene camera with default settings
        configureSceneCamera();

        // Run Lua-script to configure scene camera with scene-specific settings
        // Requires core bindings and default scene camera configuration
        config.luaEngine->configureSceneCamera(config.scene);

        // Run Lua-script to create the entities for the scene and initialize them with components
        // Requires core bindings and scene camera configuration
        config.luaEngine->createEntities(config.scene);

        // Bindings to entities' associated Lua-script to be invoked each game update
        // Requires entities to have been created
        config.luaEngine->initializeEntityBindings(this);

        // Calculate transforms for entities in the scene
        // Requires entities to have been created
        for (const entt::entity entity : entityRegistry.view<TransformComponent>()) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            calculateTranslation(&transformComponent);
            // calculateRotation(&transformComponent);
            // calculateRotationOffsets(&transformComponent);
            calculateScale(&transformComponent);
        }

        // Load meshes for entities in the scene
        // Requires entities to have been created
        for (const entt::entity entity : entityRegistry.view<MeshComponent>()) {
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
            auto mesh = config.meshManager->getMesh(meshComponent.meshInfo);

            // glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            // for (Vertex& vertex : mesh->vertices) {
            //     glm::vec4 transformedVertex = rotationMatrix * glm::vec4(vertex.position, 1.0f);
            //     vertex = glm::vec3(transformedVertex);
            // }
            meshComponent.mesh = mesh;
        }
    }

    void Scene::terminateScene() {
        entityRegistry.clear();
    }

    void Scene::resetSceneCamera() const {
        configureSceneCamera();
        config.luaEngine->configureSceneCamera(config.scene);
    }

    void Scene::configureSceneCamera() const {
        // Align directions with world space coordinate system (right-handed):
        // - Positive X --> Right
        // - Positive Y --> Up
        // - Positive Z --> Towards viewer / Out of screen
        glm::vec3 worldUp = POSITIVE_Y_AXIS;
        glm::vec3 forward = NEGATIVE_Z_AXIS;
        glm::vec3 right = POSITIVE_X_AXIS;
        glm::vec3 up = POSITIVE_Y_AXIS;

        glm::mat3 rotationMatrix;
        rotationMatrix[0] = right;
        rotationMatrix[1] = up;
        rotationMatrix[2] = forward;

        glm::quat orientation = glm::toQuat(rotationMatrix);
        glm::vec3 eulerAngles = glm::eulerAngles(orientation);
        eulerAngles = glm::degrees(eulerAngles);

        config.sceneCamera->position = {0.0f, 0.0f, 0.0f};
        config.sceneCamera->moveSpeed = 1.0f;
        config.sceneCamera->rotationSpeed = 1.0f;
        config.sceneCamera->fieldOfView = 45.0f;
        config.sceneCamera->nearClip = 0.1f;
        config.sceneCamera->farClip = 100.0f;
        config.sceneCamera->worldUpDirection = worldUp;
        config.sceneCamera->forwardDirection = forward;
        config.sceneCamera->rightDirection = right;
        config.sceneCamera->upDirection = up;
        config.sceneCamera->yaw = eulerAngles.y;
        config.sceneCamera->pitch = eulerAngles.x;
        config.sceneCamera->roll = eulerAngles.z;
    }

    void Scene::calculateTranslation(TransformComponent* transformComponent) const {
        transformComponent->translation = glm::translate(glm::mat4(1.0f), transformComponent->position);
    }

    void Scene::calculateScale(TransformComponent* transformComponent) const {
        transformComponent->scale = glm::scale(glm::mat4(1.0f), transformComponent->size);
    }

    void Scene::calculateRotation(TransformComponent* transformComponent) const {
        float yawRadians = glm::radians(transformComponent->yaw);
        float pitchRadians = glm::radians(transformComponent->pitch);
        float rollRadians = glm::radians(transformComponent->roll);

        glm::quat qYaw = glm::angleAxis(yawRadians, glm::vec3(0.0f, 1.0f, 0.0f));    // Yaw around y-axis
        glm::quat qPitch = glm::angleAxis(pitchRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch around x-axis
        glm::quat qRoll = glm::angleAxis(rollRadians, glm::vec3(0.0f, 0.0f, 1.0f));  // Roll around z-axis

        // Combine the quaternions (order matters, depending on your desired rotation sequence)
        glm::quat orientation = qYaw * qPitch;

        transformComponent->orientation = orientation;
        transformComponent->rotation = glm::toMat4(transformComponent->orientation);
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

        glm::quat yawOffsetOrientation = glm::angleAxis(yawOffsetRadians, POSITIVE_Y_AXIS);
        glm::quat pitchOffsetOrientation = glm::angleAxis(pitchOffsetRadians, POSITIVE_X_AXIS);
        glm::quat rollOffsetOrientation = glm::angleAxis(rollOffsetRadians, POSITIVE_Z_AXIS);

        transformComponent->orientationOffset = yawOffsetOrientation * pitchOffsetOrientation * rollOffsetOrientation;
    }
}
