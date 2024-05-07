#include "pch.h"
#include "Scene.h"
#include "Components.h"
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
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Num_1) {
            useSceneCamera = true;
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Num_2) {
            //useSceneCamera = false;
            return;
        }
        config.sceneCamera->onEvent(event);
    }

    void Scene::update(double timestep) {
        config.luaEngine->updateEntityBindings(&registry, timestep);

        entt::entity playerEntity = entt::null;
        for (auto entity : registry.view<TagComponent>()) {
            if (registry.get<TagComponent>(entity).tag == "Player") {
                playerEntity = entity;
                break;
            }
        }
        if (playerEntity == entt::null) {
            BL_THROW("Could not find player entity");
        }

        // const entt::entity& cameraEntity = registry.view<CameraComponent>().front();
        // if (cameraEntity == entt::null) {
        //     BL_THROW("Could not find camera entity");
        // }

        // for (const entt::entity entity : registry.view<TransformComponent>()) {
        //     auto& transformComponent = registry.get<TransformComponent>(entity);
        //     transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
        //     //transformComponent.rotation = transformComponent.getRotation();
        // }

        auto& playerTransformComponent = registry.get<TransformComponent>(playerEntity);

        playerTransformComponent.forwardDirection.x = cos(glm::radians(playerTransformComponent.yaw)) * cos(glm::radians(playerTransformComponent.pitch));
        playerTransformComponent.forwardDirection.y = sin(glm::radians(playerTransformComponent.pitch));
        playerTransformComponent.forwardDirection.z = sin(glm::radians(playerTransformComponent.yaw)) * cos(glm::radians(playerTransformComponent.pitch));
        playerTransformComponent.forwardDirection = glm::normalize(playerTransformComponent.forwardDirection);

        playerTransformComponent.rightDirection = glm::normalize(glm::cross(playerTransformComponent.forwardDirection, playerTransformComponent.worldUpDirection));
        playerTransformComponent.upDirection = glm::normalize(glm::cross(playerTransformComponent.rightDirection, playerTransformComponent.forwardDirection));

        // glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(playerTransformComponent.roll), playerTransformComponent.forwardDirection);
        // playerTransformComponent.upDirection = glm::vec3(rollMatrix * glm::vec4(playerTransformComponent.upDirection, 0.0f));
        // playerTransformComponent.rightDirection = glm::vec3(rollMatrix * glm::vec4(playerTransformComponent.rightDirection, 0.0f));

        playerTransformComponent.translation = glm::translate(glm::mat4(1.0f), playerTransformComponent.position);
        playerTransformComponent.rotation = playerTransformComponent.getRotation1();

        // playerTransformComponent.logState();

        //auto& cameraTransformComponent = registry.get<TransformComponent>(cameraEntity);
        //cameraTransformComponent.rotation = playerTransformComponent.getRotation();//config.sceneCamera->getRotation();
        // auto& cameraComponent = registry.get<CameraComponent>(cameraEntity);
        // cameraComponent.view = glm::lookAt(cameraTransformComponent.position, cameraTransformComponent.position + cameraTransformComponent.forwardDirection, cameraTransformComponent.upDirection);
        // cameraComponent.projection = config.sceneCamera->projection;

        for (const entt::entity entity : registry.view<TransformComponent, MeshComponent>()) {
            auto& transformComponent = registry.get<TransformComponent>(entity);
            auto& meshComponent = registry.get<MeshComponent>(entity);
            transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(transformComponent.yawOffset), {0.0f, 1.0f, 0.0f});
            meshComponent.mesh->model = transformComponent.translation * transformComponent.rotation * transformComponent.scale;
        }

        config.sceneCamera->update(timestep);
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
        // config.sceneCamera->position = {-3, 1, 0};
        // config.sceneCamera->moveSpeed = 5;
        // config.sceneCamera->rotationSpeed = 2.5;

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

            // transformComponent.yaw = std::atan2(transformComponent.forwardDirection.x, transformComponent.forwardDirection.z);
            // transformComponent.pitch = std::asin(transformComponent.forwardDirection.y);
            // transformComponent.roll = std::atan2(transformComponent.rightDirection.y, transformComponent.upDirection.y);
            //
            // transformComponent.yaw = 90;

            MeshInfo meshInfo{};

            meshInfo.modelPath = "models/fighter/fighter.obj";
            meshInfo.textureAtlasPath = "models/fighter/crono782.jpg";
            //transformComponent.yawOffset = 180.0f;

            // meshInfo.modelPath = "models/viking_room/viking_room.obj";
            // meshInfo.textureAtlasPath = "models/viking_room/viking_room.png";
            // transformComponent.pitchOffset = 90.0f;

            // meshInfo.modelPath = "models/camera/11673_camera_v1_L3.obj";
            // meshInfo.texturesDirectoryPath = "models/camera";
            // // transformComponent.pitchOffset = 90.0f;
            // transformComponent.yawOffset = -90.0f;
            // transformComponent.rollOffset = -90.0f;
            // transformComponent.scale = glm::scale(glm::mat4(1.0f), {0.05, 0.05, 0.05});

            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Player";
            registry.emplace<TagComponent>(entity, tagComponent);

            LuaComponent luaComponent{};
            luaComponent.type = "Player";
            luaComponent.path = "lua/player.out";
            registry.emplace<LuaComponent>(entity, luaComponent);

            MeshComponent meshComponent{};
            meshComponent.mesh = config.meshManager->getMesh(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
        // Camera
        // {
        //     entt::entity entity = registry.create();
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = {0, 0, 0};
        //     transformComponent.forwardDirection = {0, 0, -1};
        //     transformComponent.upDirection = {0, 1, 0};
        //     transformComponent.worldUpDirection = {0, 1, 0};
        //     transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
        //     transformComponent.rotation = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), {0.0f, 1.0f, 0.0f});
        //     transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(270.0f), {1.0f, 0.0f, 0.0f});
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
        //     //registry.emplace<MeshComponent>(entity, meshComponent);
        // }
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
