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
    const glm::vec3 Scene::X_AXIS = { 1.0f, 0.0f, 0.0f };
    const glm::vec3 Scene::Y_AXIS = { 0.0f, 1.0f, 0.0f };
    const glm::vec3 Scene::Z_AXIS = { 0.0f, 0.0f, 1.0f };

    Scene::Scene(const SceneConfig& config) : config(config), useSceneCamera(true) {
        config.luaEngine->initializeCoreLuaBindings(this);
        config.luaEngine->initializeEntities(config.scene);
        config.luaEngine->initializeEntityLuaBindings(this);
        calculateTransformsPostInitialization();
        loadEntityMeshes();
    }

    Scene::~Scene() {
        entityRegistry.clear();
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
            config.luaEngine->initializeEntityLuaBindings(this);
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::T) {
            config.renderer->waitUntilIdle();
            entityRegistry.clear();
            config.meshManager->resetDescriptors();
            config.luaEngine->compileLuaFiles();
            config.luaEngine->initializeEntities(config.scene);
            config.luaEngine->initializeEntityLuaBindings(this);
            calculateTransformsPostInitialization();
            loadEntityMeshes();
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

    void Scene::calculateTranslation(TransformComponent* transformComponent) const {
        transformComponent->translation = glm::translate(glm::mat4(1.0f), transformComponent->position);
    }

    void Scene::calculateRotation(TransformComponent* transformComponent) const {
        // --------------------------------------------------------------------------------------------------------------
        // Calculate the new orientation of the mesh
        // --------------------------------------------------------------------------------------------------------------

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

        // --------------------------------------------------------------------------------------------------------------
        // Calculate extra model space offsets
        // --------------------------------------------------------------------------------------------------------------
        // Models may use different coordinate systems in model space. This makes their rotation inconsistent with
        // their transform when they are moved into world space.
        //
        // For example, a model may be facing the _positive_ Z-axis even though the transform says it's facing the
        // _negative_ Z-axis (i.e. it's facing the wrong way). In this case, we apply a "model space offset" of
        // 180 degrees yaw to make it face the correct way.
        // --------------------------------------------------------------------------------------------------------------

        // float yawOffsetRadians = glm::radians(transformComponent->yawOffset);
        // float pitchOffsetRadians = glm::radians(transformComponent->pitchOffset);
        // float rollOffsetRadians = glm::radians(transformComponent->rollOffset);
        //
        // // Calculate quaternions for yaw, pitch, and roll rotation offsets
        // glm::quat yawOffsetOrientation = glm::angleAxis(yawOffsetRadians, Y_AXIS);
        // glm::quat pitchOffsetOrientation = glm::angleAxis(pitchOffsetRadians, X_AXIS);
        // glm::quat rollOffsetOrientation = glm::angleAxis(rollOffsetRadians, Z_AXIS);
        //
        // // Combine quaternions to obtain the final orientation quaternion
        // glm::quat modelSpaceOffsetOrientation = yawOffsetOrientation * pitchOffsetOrientation * rollOffsetOrientation;

        // --------------------------------------------------------------------------------------------------------------
        // Calculate the final rotation
        // --------------------------------------------------------------------------------------------------------------

        transformComponent->rotation = glm::toMat4(transformComponent->orientation * transformComponent->orientationOffset);
    }

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

    void Scene::calculateScale(TransformComponent* transformComponent) const {
        transformComponent->scale = glm::scale(glm::mat4(1.0f), transformComponent->size);
    }

    void Scene::render() {
        ViewProjection viewProjection{};
        if (useSceneCamera) {
            viewProjection.view = config.sceneCamera->getView();
            viewProjection.projection = config.sceneCamera->getProjection();
        } else {
            const entt::entity& cameraEntity = entityRegistry.view<CameraComponent>().front();
            if (cameraEntity == entt::null) {
                BL_THROW("Could not find camera entity");
            }
            const auto& cameraComponent = entityRegistry.get<CameraComponent>(cameraEntity);
            viewProjection.view = cameraComponent.view;
            viewProjection.projection = cameraComponent.projection;
        }
        for (const entt::entity entity : entityRegistry.view<MeshComponent, TagComponent>()) {
            auto& tagComponent = entityRegistry.get<TagComponent>(entity);
            if (tagComponent.tag == "Camera" && !useSceneCamera) {
                continue;
            }
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
            config.renderer->renderMesh(meshComponent.mesh, viewProjection);
        }
    }

    void Scene::calculateTransformsPostInitialization() {
        for (const entt::entity entity : entityRegistry.view<TransformComponent>()) {
            auto& transformComponent = entityRegistry.get<TransformComponent>(entity);
            calculateTranslation(&transformComponent);
            calculateRotation(&transformComponent);
            calculateRotationOffsets(&transformComponent);
            calculateScale(&transformComponent);
        }
    }

    void Scene::loadEntityMeshes() {
        for (const entt::entity entity : entityRegistry.view<MeshComponent>()) {
            auto& meshComponent = entityRegistry.get<MeshComponent>(entity);
            meshComponent.mesh = config.meshManager->getMesh(meshComponent.meshInfo);
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

    void Scene::initializeCamera() const {
        glm::vec3 worldUpDirection = Y_AXIS;
        glm::vec3 forwardDirection = {Z_AXIS.x, Z_AXIS.y, -Z_AXIS.z};
        glm::vec3 rightDirection = glm::normalize(glm::cross(forwardDirection, worldUpDirection));
        glm::vec3 upDirection = glm::normalize(glm::cross(rightDirection, forwardDirection));

        float yaw = glm::degrees(std::atan2(forwardDirection.z, forwardDirection.x));
        float pitch = glm::degrees(std::asin(-forwardDirection.y));
        float roll = glm::degrees(std::atan2(rightDirection.y, upDirection.y));

        config.sceneCamera->worldUpDirection = worldUpDirection;
        config.sceneCamera->forwardDirection = forwardDirection;
        config.sceneCamera->rightDirection = rightDirection;
        config.sceneCamera->upDirection = upDirection;
        config.sceneCamera->yaw = yaw;
        config.sceneCamera->pitch = pitch;
        config.sceneCamera->roll = roll;

        config.sceneCamera->frustum = {
            .fieldOfView = glm::radians(45.0f),
            .nearClip = 0.1f,
            .farClip = 10000.0f,
        };
    }

    void Scene::createEntities() {
        //--------------------------------------------------------------------------------------------------------------
        // Player
        //--------------------------------------------------------------------------------------------------------------
        // {
        //     entt::entity entity = registry.create();
        //
        //     FighterJetComponent fighterJetComponent{};
        //     fighterJetComponent.type = "crono782";
        //     registry.emplace<FighterJetComponent>(entity, fighterJetComponent);
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = {0, 0, 0};
        //     transformComponent.worldUpDirection = {0, 1, 0};
        //     transformComponent.forwardDirection = {0, 0, -1};
        //     transformComponent.yaw = -90.0f;
        //     transformComponent.yawOffset = 180.0f;
        //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        //     registry.emplace<TransformComponent>(entity, transformComponent);
        //
        //     TagComponent tagComponent{};
        //     tagComponent.tag = "Player";
        //     registry.emplace<TagComponent>(entity, tagComponent);
        //
        //     LuaComponent luaComponent{};
        //     luaComponent.type = "Player";
        //     luaComponent.path = "lua/player.out";
        //     registry.emplace<LuaComponent>(entity, luaComponent);
        //
        //     MeshInfo meshInfo{};
        //     meshInfo.modelPath = "models/fighter/fighter.obj";
        //     meshInfo.textureAtlasPath = "models/fighter/" + fighterJetComponent.type + ".jpg";
        //
        //     MeshComponent meshComponent{};
        //     meshComponent.meshInfo = meshInfo;
        //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        //     registry.emplace<MeshComponent>(entity, meshComponent);
        // }
        // //--------------------------------------------------------------------------------------------------------------
        // // Camera
        // //--------------------------------------------------------------------------------------------------------------
        // {
        //     entt::entity entity = registry.create();
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = {0, 2, 0};
        //
        //     transformComponent.worldUpDirection = {0, 1, 0};
        //     transformComponent.forwardDirection = {0, 0, -1};
        //     transformComponent.yaw = -90.0f;
        //     // transformComponent.yawOffset = 180.0f;
        //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        //
        //     transformComponent.yawOffset = 90;
        //     transformComponent.pitchOffset = -90;
        //     // transformComponent.rotation = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), {0.0f, 1.0f, 0.0f});
        //     // transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(270.0f), {1.0f, 0.0f, 0.0f});
        //     transformComponent.scale = glm::scale(glm::mat4(1.0f), {0.05, 0.05, 0.05});
        //     registry.emplace<TransformComponent>(entity, transformComponent);
        //
        //     TagComponent tagComponent{};
        //     tagComponent.tag = "Camera";
        //     registry.emplace<TagComponent>(entity, tagComponent);
        //
        //     LuaComponent luaComponent{};
        //     luaComponent.type = "Camera";
        //     luaComponent.path = "lua/camera.out";
        //     registry.emplace<LuaComponent>(entity, luaComponent);
        //
        //     CameraComponent cameraComponent{};
        //     cameraComponent.view = config.sceneCamera->getView();
        //     cameraComponent.projection = config.sceneCamera->getProjection();
        //     registry.emplace<CameraComponent>(entity, cameraComponent);
        //
        //     MeshInfo meshInfo{};
        //     meshInfo.modelPath = "models/camera/11673_camera_v1_L3.obj";
        //     meshInfo.texturesDirectoryPath = "models/camera";
        //
        //     MeshComponent meshComponent{};
        //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        //     registry.emplace<MeshComponent>(entity, meshComponent);
        // }
        // //--------------------------------------------------------------------------------------------------------------
        // // Sibenik cathedral
        // //--------------------------------------------------------------------------------------------------------------
        // {
        //     entt::entity entity = registry.create();
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = {-161.29372, -7.5, -54.056705};
        //     transformComponent.worldUpDirection = {0, 1, 0};
        //     transformComponent.forwardDirection = {0, 0, -1};
        //     transformComponent.yaw = -90.0f;
        //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        //     transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
        //     registry.emplace<TransformComponent>(entity, transformComponent);
        //
        //     TagComponent tagComponent{};
        //     tagComponent.tag = "Sibenik";
        //     registry.emplace<TagComponent>(entity, tagComponent);
        //
        //     MeshInfo meshInfo{};
        //     meshInfo.modelPath = "models/sibenik/sibenik.obj";
        //     meshInfo.texturesDirectoryPath = "models/sibenik";
        //
        //     MeshComponent meshComponent{};
        //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        //     registry.emplace<MeshComponent>(entity, meshComponent);
        // }
        // //--------------------------------------------------------------------------------------------------------------
        // // Terrain
        // //--------------------------------------------------------------------------------------------------------------
        // {
        //     entt::entity entity = registry.create();
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = {0, -50, 0};
        //     transformComponent.worldUpDirection = {0, 1, 0};
        //     transformComponent.forwardDirection = {0, 0, -1};
        //     transformComponent.yaw = -90.0f;
        //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        //     transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
        //     registry.emplace<TransformComponent>(entity, transformComponent);
        //
        //     TagComponent tagComponent{};
        //     tagComponent.tag = "Terrain";
        //     registry.emplace<TagComponent>(entity, tagComponent);
        //
        //     MeshInfo meshInfo{};
        //     meshInfo.modelPath = "models/terrain/terrain.obj";
        //     meshInfo.textureAtlasPath = "models/terrain/diff2.jpg";
        //
        //     MeshComponent meshComponent{};
        //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        //     registry.emplace<MeshComponent>(entity, meshComponent);
        // }
        // //--------------------------------------------------------------------------------------------------------------
        // // Viking room
        // //--------------------------------------------------------------------------------------------------------------
        // {
        //     entt::entity entity = registry.create();
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = {-100, -25, -900};
        //     transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
        //
        //     transformComponent.worldUpDirection = {0, 1, 0};
        //     transformComponent.forwardDirection = {0, 0, -1};
        //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        //
        //     transformComponent.yaw = -225.0f;
        //
        //     transformComponent.pitchOffset = -90.0f;
        //
        //     transformComponent.scale = glm::scale(glm::mat4(1.0f), {100, 100, 100});
        //
        //     registry.emplace<TransformComponent>(entity, transformComponent);
        //
        //     TagComponent tagComponent{};
        //     tagComponent.tag = "Viking Room";
        //     registry.emplace<TagComponent>(entity, tagComponent);
        //
        //     MeshInfo meshInfo{};
        //     meshInfo.modelPath = "models/viking_room/viking_room.obj";
        //     meshInfo.textureAtlasPath = "models/viking_room/viking_room.png";
        //
        //     MeshComponent meshComponent{};
        //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        //     registry.emplace<MeshComponent>(entity, meshComponent);
        // }
        // //--------------------------------------------------------------------------------------------------------------
        // // Freighter
        // //--------------------------------------------------------------------------------------------------------------
        // {
        //     entt::entity entity = registry.create();
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = {-405.73834, 260, -577.0076};
        //     transformComponent.worldUpDirection = {0, 1, 0};
        //     transformComponent.forwardDirection = {0, 0, -1};
        //     transformComponent.yaw = -90.0f;
        //     transformComponent.yawOffset = 180.0f;
        //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        //
        //     transformComponent.scale = glm::scale(glm::mat4(1.0f), {10, 10, 10});
        //     registry.emplace<TransformComponent>(entity, transformComponent);
        //
        //     TagComponent tagComponent{};
        //     tagComponent.tag = "Mothership";
        //     registry.emplace<TagComponent>(entity, tagComponent);
        //
        //     MeshInfo meshInfo{};
        //     meshInfo.modelPath = "models/oxar_freighter/Meshes/oxar_freighter.obj";
        //     meshInfo.textureAtlasPath = "models/oxar_freighter/Textures/Oxar_Diffuse.png";
        //
        //     MeshComponent meshComponent{};
        //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        //     registry.emplace<MeshComponent>(entity, meshComponent);
        // }
        // //--------------------------------------------------------------------------------------------------------------
        // // Circle patrol squadron
        // //--------------------------------------------------------------------------------------------------------------
        // //glm::vec3 squadronLeaderPosition = {50, 50, -100};
        // //glm::vec3 squadronLeaderPosition = {-380.0f, 400.0f, -490.0f};
        // // glm::vec3 squadronLeaderPosition = {-700.0f, 140.0f, -500.0f};
        // // float squadronLeaderScale = 100.0f;
        // // {
        // //     entt::entity entity = registry.create();
        // //
        // //     FighterJetComponent fighterJetComponent{};
        // //     fighterJetComponent.type = "krulspeld1";
        // //     registry.emplace<FighterJetComponent>(entity, fighterJetComponent);
        // //
        // //     TransformComponent transformComponent{};
        // //     transformComponent.position = squadronLeaderPosition;
        // //     transformComponent.worldUpDirection = {0, 1, 0};
        // //     transformComponent.forwardDirection = {0, 0, -1};
        // //     transformComponent.yaw = 00.0f;
        // //     transformComponent.yawOffset = 180.0f;
        // //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        // //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        // //
        // //     transformComponent.scale = glm::scale(glm::mat4(1.0f), {squadronLeaderScale, squadronLeaderScale, squadronLeaderScale});
        // //     registry.emplace<TransformComponent>(entity, transformComponent);
        // //
        // //     TagComponent tagComponent{};
        // //     tagComponent.tag = "Circle patrol 1";
        // //     registry.emplace<TagComponent>(entity, tagComponent);
        // //
        // //     LuaComponent luaComponent{};
        // //     luaComponent.type = "AiPatrolCircle";
        // //     luaComponent.path = "lua/ai_patrol_circle.out";
        // //     registry.emplace<LuaComponent>(entity, luaComponent);
        // //
        // //     MeshInfo meshInfo{};
        // //     meshInfo.modelPath = "models/fighter/fighter.obj";
        // //     meshInfo.textureAtlasPath = "models/fighter/" + fighterJetComponent.type + ".jpg";
        // //
        // //     MeshComponent meshComponent{};
        // //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        // //     registry.emplace<MeshComponent>(entity, meshComponent);
        // // }
        // // {
        // //     entt::entity entity = registry.create();
        // //
        // //     FighterJetComponent fighterJetComponent{};
        // //     fighterJetComponent.type = "krulspeld2";
        // //     registry.emplace<FighterJetComponent>(entity, fighterJetComponent);
        // //
        // //     TransformComponent transformComponent{};
        // //     transformComponent.position = { squadronLeaderPosition.x + squadronLeaderScale, squadronLeaderPosition.y, squadronLeaderPosition.z + squadronLeaderScale};
        // //     transformComponent.worldUpDirection = {0, 1, 0};
        // //     transformComponent.forwardDirection = {0, 0, -1};
        // //     transformComponent.yaw = 00.0f;
        // //     transformComponent.yawOffset = 180.0f;
        // //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        // //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        // //
        // //     transformComponent.scale = glm::scale(glm::mat4(1.0f), {squadronLeaderScale, squadronLeaderScale, squadronLeaderScale});
        // //     registry.emplace<TransformComponent>(entity, transformComponent);
        // //
        // //     TagComponent tagComponent{};
        // //     tagComponent.tag = "Circle patrol 2";
        // //     registry.emplace<TagComponent>(entity, tagComponent);
        // //
        // //     LuaComponent luaComponent{};
        // //     luaComponent.type = "AiPatrolCircle";
        // //     luaComponent.path = "lua/ai_patrol_circle.out";
        // //     registry.emplace<LuaComponent>(entity, luaComponent);
        // //
        // //     MeshInfo meshInfo{};
        // //     meshInfo.modelPath = "models/fighter/fighter.obj";
        // //     meshInfo.textureAtlasPath = "models/fighter/" + fighterJetComponent.type + ".jpg";
        // //
        // //     MeshComponent meshComponent{};
        // //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        // //     registry.emplace<MeshComponent>(entity, meshComponent);
        // // }
        // // {
        // //     entt::entity entity = registry.create();
        // //
        // //     FighterJetComponent fighterJetComponent{};
        // //     fighterJetComponent.type = "krulspeld3";
        // //     registry.emplace<FighterJetComponent>(entity, fighterJetComponent);
        // //
        // //     TransformComponent transformComponent{};
        // //     transformComponent.position = { squadronLeaderPosition.x + squadronLeaderScale, squadronLeaderPosition.y, squadronLeaderPosition.z - squadronLeaderScale};
        // //     transformComponent.worldUpDirection = {0, 1, 0};
        // //     transformComponent.forwardDirection = {0, 0, -1};
        // //     transformComponent.yaw = 00.0f;
        // //     transformComponent.yawOffset = 180.0f;
        // //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        // //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        // //
        // //     transformComponent.scale = glm::scale(glm::mat4(1.0f), {squadronLeaderScale, squadronLeaderScale, squadronLeaderScale});
        // //     registry.emplace<TransformComponent>(entity, transformComponent);
        // //
        // //     TagComponent tagComponent{};
        // //     tagComponent.tag = "Circle patrol 3";
        // //     registry.emplace<TagComponent>(entity, tagComponent);
        // //
        // //     LuaComponent luaComponent{};
        // //     luaComponent.type = "AiPatrolCircle";
        // //     luaComponent.path = "lua/ai_patrol_circle.out";
        // //     registry.emplace<LuaComponent>(entity, luaComponent);
        // //
        // //     MeshInfo meshInfo{};
        // //     meshInfo.modelPath = "models/fighter/fighter.obj";
        // //     meshInfo.textureAtlasPath = "models/fighter/" + fighterJetComponent.type + ".jpg";
        // //
        // //     MeshComponent meshComponent{};
        // //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        // //     registry.emplace<MeshComponent>(entity, meshComponent);
        // // }
        // //--------------------------------------------------------------------------------------------------------------
        // // Line patrol squadron
        // //--------------------------------------------------------------------------------------------------------------
        // {
        //     entt::entity entity = registry.create();
        //
        //     FighterJetComponent fighterJetComponent{};
        //     fighterJetComponent.type = "krulspeld4";
        //     registry.emplace<FighterJetComponent>(entity, fighterJetComponent);
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = { -50, 50, -50 };
        //     transformComponent.worldUpDirection = {0, 1, 0};
        //     transformComponent.forwardDirection = {-1, 0, 0};
        //     transformComponent.yaw = -180.0f;
        //     transformComponent.yawOffset = 180.0f;
        //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        //
        //     transformComponent.scale = glm::scale(glm::mat4(1.0f), {100, 100, 100});
        //     registry.emplace<TransformComponent>(entity, transformComponent);
        //
        //     TagComponent tagComponent{};
        //     tagComponent.tag = "Line patrol 1";
        //     registry.emplace<TagComponent>(entity, tagComponent);
        //
        //     LuaComponent luaComponent{};
        //     luaComponent.type = "AiPatrolLine";
        //     luaComponent.path = "lua/ai_patrol_line.out";
        //     registry.emplace<LuaComponent>(entity, luaComponent);
        //
        //     MeshInfo meshInfo{};
        //     meshInfo.modelPath = "models/fighter/fighter.obj";
        //     meshInfo.textureAtlasPath = "models/fighter/" + fighterJetComponent.type + ".jpg";
        //
        //     MeshComponent meshComponent{};
        //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        //     registry.emplace<MeshComponent>(entity, meshComponent);
        // }
        // {
        //     entt::entity entity = registry.create();
        //
        //     FighterJetComponent fighterJetComponent{};
        //     fighterJetComponent.type = "krulspeld5";
        //     registry.emplace<FighterJetComponent>(entity, fighterJetComponent);
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = { -50, 50, -150 };
        //     transformComponent.worldUpDirection = {0, 1, 0};
        //     transformComponent.forwardDirection = {-1, 0, 0};
        //     transformComponent.yaw = -180.0f;
        //     transformComponent.yawOffset = 180.0f;
        //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        //
        //     transformComponent.scale = glm::scale(glm::mat4(1.0f), {100, 100, 100});
        //     registry.emplace<TransformComponent>(entity, transformComponent);
        //
        //     TagComponent tagComponent{};
        //     tagComponent.tag = "Line patrol 2";
        //     registry.emplace<TagComponent>(entity, tagComponent);
        //
        //     LuaComponent luaComponent{};
        //     luaComponent.type = "AiPatrolLine";
        //     luaComponent.path = "lua/ai_patrol_line.out";
        //     registry.emplace<LuaComponent>(entity, luaComponent);
        //
        //     MeshInfo meshInfo{};
        //     meshInfo.modelPath = "models/fighter/fighter.obj";
        //     meshInfo.textureAtlasPath = "models/fighter/" + fighterJetComponent.type + ".jpg";
        //
        //     MeshComponent meshComponent{};
        //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        //     registry.emplace<MeshComponent>(entity, meshComponent);
        // }
    }

    void Scene::destroyEntities() {
        entityRegistry.clear();
    }
}
