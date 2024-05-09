#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "glm/gtx/rotate_vector.hpp"
#include "window/KeyEvent.h"
#include "graphics/Mesh.h"
#include "graphics/ViewProjection.h"

namespace Blink {
    Scene::Scene(const SceneConfig& config) : config(config), useSceneCamera(true) {
        initializeScene();
        config.luaEngine->createEntityBindings(&registry);
    }

    Scene::~Scene() {
        registry.clear();
    }

    void Scene::onEvent(Event& event) {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::F1) {
            config.luaEngine->reloadScripts(&registry);
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Num_0) {
            config.sceneCamera->inputEnabled = !config.sceneCamera->inputEnabled;
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Num_1) {
            useSceneCamera = true;
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Num_2) {
            useSceneCamera = false;
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::R) {
            for (auto entity : registry.view<TransformComponent, TagComponent>()) {
                auto& tagComponent = registry.get<TagComponent>(entity);
                if (tagComponent.tag != "Player") {
                    continue;
                }
                auto& transformComponent = registry.get<TransformComponent>(entity);
                transformComponent.upDirection = transformComponent.worldUpDirection;
                transformComponent.forwardDirection = {0, 0, -1};
                transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
                transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
                transformComponent.yaw = -90.0f;
                transformComponent.pitch = 0;
                transformComponent.roll = 0;
            }
            return;
        }
        config.sceneCamera->onEvent(event);
    }

    void Scene::update(double timestep) {
        config.luaEngine->updateEntityBindings(&registry, timestep);

        for (const entt::entity entity : registry.view<TransformComponent>()) {
            auto& transformComponent = registry.get<TransformComponent>(entity);

            transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);

            // transformComponent.forwardDirection.x = cos(glm::radians(transformComponent.yaw)) * cos(glm::radians(transformComponent.pitch));
            // transformComponent.forwardDirection.y = sin(glm::radians(transformComponent.pitch));
            // transformComponent.forwardDirection.z = sin(glm::radians(transformComponent.yaw)) * cos(glm::radians(transformComponent.pitch));
            // transformComponent.forwardDirection = glm::normalize(transformComponent.forwardDirection);
            //
            // transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
            // transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));

            glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(transformComponent.yaw), {0.0f, 1.0f, 0.0f});
            glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(transformComponent.pitch), transformComponent.rightDirection);
            glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(transformComponent.roll), {0.0f, 0.0f, 1.0f});

            transformComponent.rotation = glm::transpose(yawMatrix * pitchMatrix * rollMatrix);
        }

        for (const entt::entity entity : registry.view<TransformComponent, MeshComponent>()) {
            auto& transformComponent = registry.get<TransformComponent>(entity);
            auto& meshComponent = registry.get<MeshComponent>(entity);
            transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(transformComponent.yawOffset), {0.0f, 1.0f, 0.0f});
            transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(transformComponent.pitchOffset), {1.0f, 0.0f, 0.0f});
            transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(transformComponent.rollOffset), {0.0f, 0.0f, 1.0f});
            meshComponent.mesh->model = transformComponent.translation * transformComponent.rotation * transformComponent.scale;
        }

        if (useSceneCamera) {
            config.sceneCamera->update(timestep);
        } else {
            for (const entt::entity entity : registry.view<TransformComponent, CameraComponent>()) {
                auto& transformComponent = registry.get<TransformComponent>(entity);
                auto& cameraComponent = registry.get<CameraComponent>(entity);
                cameraComponent.view = glm::lookAt(transformComponent.position, transformComponent.position + transformComponent.forwardDirection, transformComponent.upDirection);
                cameraComponent.projection = config.sceneCamera->projection;
            }
        }
    }

    void Scene::render() {
        ViewProjection viewProjection{};
        if (useSceneCamera) {
            viewProjection.view = config.sceneCamera->getView();
            viewProjection.projection = config.sceneCamera->getProjection();
        } else {
            const entt::entity& cameraEntity = registry.view<CameraComponent>().front();
            if (cameraEntity == entt::null) {
                BL_THROW("Could not find camera entity");
            }
            const auto& cameraComponent = registry.get<CameraComponent>(cameraEntity);
            viewProjection.view = cameraComponent.view;
            viewProjection.projection = cameraComponent.projection;
        }
        for (const entt::entity entity : registry.view<MeshComponent>()) {
            auto& meshComponent = registry.get<MeshComponent>(entity);
            config.renderer->renderMesh(meshComponent.mesh, viewProjection);
        }
    }

    void Scene::initializeScene() {
        // Scene camera
        config.sceneCamera->position = {0, 0, 0};
        config.sceneCamera->moveSpeed = 5;
        config.sceneCamera->rotationSpeed = 1;
        config.sceneCamera->yaw = -90.0f;
        config.sceneCamera->worldUpDirection = {0, 1, 0};
        config.sceneCamera->forwardDirection = {0, 0, -1};
        config.sceneCamera->rightDirection = glm::normalize(glm::cross(config.sceneCamera->forwardDirection, config.sceneCamera->worldUpDirection));
        config.sceneCamera->upDirection = glm::normalize(glm::cross(config.sceneCamera->rightDirection, config.sceneCamera->forwardDirection));

        // Player
        {
            entt::entity entity = registry.create();

            TransformComponent transformComponent{};
            transformComponent.position = {0, 0, 0};
            transformComponent.worldUpDirection = {0, 1, 0};
            transformComponent.forwardDirection = {0, 0, -1};
            transformComponent.yaw = -90.0f;
            transformComponent.yawOffset = 180.0f;
            transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
            transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Player";
            registry.emplace<TagComponent>(entity, tagComponent);

            LuaComponent luaComponent{};
            luaComponent.type = "Player";
            luaComponent.path = "lua/player.out";
            registry.emplace<LuaComponent>(entity, luaComponent);

            MeshInfo meshInfo{};
            meshInfo.modelPath = "models/fighter/fighter.obj";
            meshInfo.textureAtlasPath = "models/fighter/crono782.jpg";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.meshManager->getMesh(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
        // Camera
        {
            entt::entity entity = registry.create();

            TransformComponent transformComponent{};
            transformComponent.position = {0, 0, 0};

            transformComponent.worldUpDirection = {0, 1, 0};
            transformComponent.forwardDirection = {0, 0, -1};
            transformComponent.yaw = -90.0f;
            // transformComponent.yawOffset = 180.0f;
            transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
            transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));

            transformComponent.yawOffset = 90;
            transformComponent.pitchOffset = -90;
            // transformComponent.rotation = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), {0.0f, 1.0f, 0.0f});
            // transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(270.0f), {1.0f, 0.0f, 0.0f});
            transformComponent.scale = glm::scale(glm::mat4(1.0f), {0.05, 0.05, 0.05});
            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Camera";
            registry.emplace<TagComponent>(entity, tagComponent);

            LuaComponent luaComponent{};
            luaComponent.type = "Camera";
            luaComponent.path = "lua/camera.out";
            registry.emplace<LuaComponent>(entity, luaComponent);

            CameraComponent cameraComponent{};
            cameraComponent.view = config.sceneCamera->getView();
            cameraComponent.projection = config.sceneCamera->getProjection();
            registry.emplace<CameraComponent>(entity, cameraComponent);

            MeshInfo meshInfo{};
            meshInfo.modelPath = "models/camera/11673_camera_v1_L3.obj";
            meshInfo.texturesDirectoryPath = "models/camera";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.meshManager->getMesh(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
        // {
        //     entt::entity entity = registry.create();
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = {0, 0, 1};
        //     transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
        //     transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //     transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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
        {
            entt::entity entity = registry.create();

            TransformComponent transformComponent{};
            transformComponent.position = {0, 0, 0};
            transformComponent.worldUpDirection = {0, 1, 0};
            transformComponent.forwardDirection = {0, 0, -1};
            transformComponent.yaw = -90.0f;
            transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
            transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
            transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Sibenik";
            registry.emplace<TagComponent>(entity, tagComponent);

            MeshInfo meshInfo{};
            meshInfo.modelPath = "models/sibenik/sibenik.obj";
            meshInfo.texturesDirectoryPath = "models/sibenik";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.meshManager->getMesh(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
    }

    // void Scene::initializeScene() {
    //     if (config.name == "fighter_jet") {
    //         initializeFighterJetScene();
    //     } else if (config.name == "viking_room") {
    //         initializeVikingRoomScene();
    //     } else {
    //         if (config.name.empty()) {
    //             BL_THROW("Missing scene name");
    //         }
    //         BL_THROW("Unknown scene [" + config.name + "]");
    //     }
    // }

    void Scene::initializeVikingRoomScene() {
        // entt::entity entity = registry.create();
        //
        // TransformComponent transformComponent{};
        // transformComponent.position = { 0, 0, 1 };
        // transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
        // transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // registry.emplace<TransformComponent>(entity, transformComponent);
        //
        // TagComponent tagComponent{};
        // tagComponent.tag = "Viking Room";
        // registry.emplace<TagComponent>(entity, tagComponent);
        //
        // MeshInfo meshInfo{};
        // meshInfo.modelPath = "models/viking_room/viking_room.obj";
        // meshInfo.textureAtlasPath = "models/viking_room/viking_room.png";
        //
        // MeshComponent meshComponent{};
        // meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        // registry.emplace<MeshComponent>(entity, meshComponent);
    }

    void Scene::initializeFighterJetScene() {
        // // Player
        // {
        //     entt::entity entity = registry.create();
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = { 0, 0, 0 };
        //     transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
        //     registry.emplace<TransformComponent>(entity, transformComponent);
        //
        //     TagComponent tagComponent{};
        //     tagComponent.tag = "Player";
        //     registry.emplace<TagComponent>(entity, tagComponent);
        //
        //     LuaComponent luaComponent{};
        //     luaComponent.type = "Player";
        //     luaComponent.filepath = "lua/player.out";
        //     registry.emplace<LuaComponent>(entity, luaComponent);
        //
        //     MeshInfo meshInfo{};
        //     meshInfo.modelPath = "models/fighter/fighter.obj";
        //     meshInfo.textureAtlasPath = "models/fighter/crono782.jpg";
        //
        //     MeshComponent meshComponent{};
        //     meshComponent.mesh = config.meshManager->getMesh(meshInfo);
        //     registry.emplace<MeshComponent>(entity, meshComponent);
        // }
        // // Enemies
        // {
        //     entt::entity entity = registry.create();
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = { 0, 0, 1 };
        //     transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
        //     transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //     transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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
        // {
        //     entt::entity entity = registry.create();
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = { 0, 0, 0 };
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
    }
}
