#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "glm/gtx/rotate_vector.hpp"
#include "window/KeyEvent.h"
#include "graphics/Mesh.h"
#include "graphics/ViewProjection.h"

namespace Blink {
    Scene::Scene(const SceneConfig& config) : config(config), useSceneCamera(true) {
        initializeCamera();
        createEntities();
        config.luaEngine->createEntityBindings(&registry);
    }

    Scene::~Scene() {
        destroyEntities();
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
            initializeCamera();
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::T) {
            config.renderer->waitUntilIdle();
            destroyEntities();
            config.meshManager->reset();
            createEntities();
            config.luaEngine->reloadScripts(&registry);
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Y) {
            config.sceneCamera->loggingEnabled = !config.sceneCamera->loggingEnabled;
        }
        config.sceneCamera->onEvent(event);
    }

    void Scene::update(double timestep) {
        config.luaEngine->updateEntityBindings(&registry, timestep);

        for (const entt::entity entity : registry.view<TransformComponent>()) {
            auto& transformComponent = registry.get<TransformComponent>(entity);

            transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);

            glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(transformComponent.yaw), {0.0f, 1.0f, 0.0f});
            glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(transformComponent.pitch), transformComponent.rightDirection);
            glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(transformComponent.roll), {0.0f, 0.0f, 1.0f});
            transformComponent.rotation = glm::transpose(yawMatrix * pitchMatrix * rollMatrix);
        }

        for (const entt::entity entity : registry.view<TransformComponent, MeshComponent>()) {
            auto& transformComponent = registry.get<TransformComponent>(entity);
            auto& meshComponent = registry.get<MeshComponent>(entity);
            transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(transformComponent.yawRenderingOffset), {0.0f, 1.0f, 0.0f});
            transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(transformComponent.pitchRenderingOffset), {1.0f, 0.0f, 0.0f});
            transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(transformComponent.rollRenderingOffset), {0.0f, 0.0f, 1.0f});
            meshComponent.mesh->model = transformComponent.translation * transformComponent.rotation * transformComponent.scale;
        }

        if (useSceneCamera) {
            config.sceneCamera->update(timestep);
            //config.sceneCamera->logState();
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
        for (const entt::entity entity : registry.view<MeshComponent, TagComponent>()) {
            auto& tagComponent = registry.get<TagComponent>(entity);
            if (tagComponent.tag == "Camera" && !useSceneCamera) {
                continue;
            }
            auto& meshComponent = registry.get<MeshComponent>(entity);
            config.renderer->renderMesh(meshComponent.mesh, viewProjection);
        }
    }

    void Scene::initializeCamera() const {
        config.sceneCamera->position = {105.584175, 44.01467, 103.974266};
        config.sceneCamera->position = {625.0857, 183.64235, -393.7724};
        config.sceneCamera->position = //{-405.73834, 600, -577.0076};

        config.sceneCamera->worldUpDirection = {0, 1, 0};
        config.sceneCamera->forwardDirection = {0.12036891, -0.15643448, -0.9803263};
        config.sceneCamera->rightDirection = glm::normalize(glm::cross(config.sceneCamera->forwardDirection, config.sceneCamera->worldUpDirection));
        config.sceneCamera->upDirection = glm::normalize(glm::cross(config.sceneCamera->rightDirection, config.sceneCamera->forwardDirection));

        config.sceneCamera->yaw = -83;
        config.sceneCamera->pitch = -9;

        config.sceneCamera->moveSpeed = 100;
        config.sceneCamera->rotationSpeed = 1;
    }

    void Scene::createEntities() {
        //--------------------------------------------------------------------------------------------------------------
        // Player
        //--------------------------------------------------------------------------------------------------------------
        {
            entt::entity entity = registry.create();

            FighterJetComponent fighterJetComponent{};
            fighterJetComponent.type = "crono782";
            registry.emplace<FighterJetComponent>(entity, fighterJetComponent);

            TransformComponent transformComponent{};
            transformComponent.position = {0, 0, 0};
            transformComponent.worldUpDirection = {0, 1, 0};
            transformComponent.forwardDirection = {0, 0, -1};
            transformComponent.yaw = -90.0f;
            transformComponent.yawRenderingOffset = 180.0f;
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
            meshInfo.textureAtlasPath = "models/fighter/" + fighterJetComponent.type + ".jpg";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.meshManager->getMesh(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
        //--------------------------------------------------------------------------------------------------------------
        // Camera
        //--------------------------------------------------------------------------------------------------------------
        {
            entt::entity entity = registry.create();

            TransformComponent transformComponent{};
            transformComponent.position = {0, 2, 0};

            transformComponent.worldUpDirection = {0, 1, 0};
            transformComponent.forwardDirection = {0, 0, -1};
            transformComponent.yaw = -90.0f;
            // transformComponent.yawOffset = 180.0f;
            transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
            transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));

            transformComponent.yawRenderingOffset = 90;
            transformComponent.pitchRenderingOffset = -90;
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
        //--------------------------------------------------------------------------------------------------------------
        // Sibenik cathedral
        //--------------------------------------------------------------------------------------------------------------
        {
            entt::entity entity = registry.create();

            TransformComponent transformComponent{};
            transformComponent.position = {-161.29372, -7.5, -54.056705};
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
        //--------------------------------------------------------------------------------------------------------------
        // Terrain
        //--------------------------------------------------------------------------------------------------------------
        {
            entt::entity entity = registry.create();

            TransformComponent transformComponent{};
            transformComponent.position = {0, -50, 0};
            transformComponent.worldUpDirection = {0, 1, 0};
            transformComponent.forwardDirection = {0, 0, -1};
            transformComponent.yaw = -90.0f;
            transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
            transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
            transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Terrain";
            registry.emplace<TagComponent>(entity, tagComponent);

            MeshInfo meshInfo{};
            meshInfo.modelPath = "models/terrain/terrain.obj";
            meshInfo.textureAtlasPath = "models/terrain/diff2.jpg";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.meshManager->getMesh(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
        //--------------------------------------------------------------------------------------------------------------
        // Viking room
        //--------------------------------------------------------------------------------------------------------------
        {
            entt::entity entity = registry.create();

            TransformComponent transformComponent{};
            transformComponent.position = {-100, -25, -900};
            transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);

            transformComponent.worldUpDirection = {0, 1, 0};
            transformComponent.forwardDirection = {0, 0, -1};
            transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
            transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));

            transformComponent.yaw = -225.0f;

            transformComponent.pitchRenderingOffset = -90.0f;

            transformComponent.scale = glm::scale(glm::mat4(1.0f), {100, 100, 100});

            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Viking Room";
            registry.emplace<TagComponent>(entity, tagComponent);

            MeshInfo meshInfo{};
            meshInfo.modelPath = "models/viking_room/viking_room.obj";
            meshInfo.textureAtlasPath = "models/viking_room/viking_room.png";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.meshManager->getMesh(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
        //--------------------------------------------------------------------------------------------------------------
        // Freighter
        //--------------------------------------------------------------------------------------------------------------
        {
            entt::entity entity = registry.create();

            TransformComponent transformComponent{};
            transformComponent.position = {-405.73834, 260, -577.0076};
            transformComponent.worldUpDirection = {0, 1, 0};
            transformComponent.forwardDirection = {0, 0, -1};
            transformComponent.yaw = -90.0f;
            transformComponent.yawRenderingOffset = 180.0f;
            transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
            transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));

            transformComponent.scale = glm::scale(glm::mat4(1.0f), {10, 10, 10});
            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Mothership";
            registry.emplace<TagComponent>(entity, tagComponent);

            MeshInfo meshInfo{};
            meshInfo.modelPath = "models/oxar_freighter/Meshes/oxar_freighter.obj";
            meshInfo.textureAtlasPath = "models/oxar_freighter/Textures/Oxar_Diffuse.png";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.meshManager->getMesh(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
        //--------------------------------------------------------------------------------------------------------------
        // Circle patrol squadron
        //--------------------------------------------------------------------------------------------------------------
        //glm::vec3 squadronLeaderPosition = {50, 50, -100};
        //glm::vec3 squadronLeaderPosition = {-380.0f, 400.0f, -490.0f};
        glm::vec3 squadronLeaderPosition = {-700.0f, 140.0f, -500.0f};
        float squadronLeaderScale = 100.0f;
        {
            entt::entity entity = registry.create();

            FighterJetComponent fighterJetComponent{};
            fighterJetComponent.type = "krulspeld1";
            registry.emplace<FighterJetComponent>(entity, fighterJetComponent);

            TransformComponent transformComponent{};
            transformComponent.position = squadronLeaderPosition;
            transformComponent.worldUpDirection = {0, 1, 0};
            transformComponent.forwardDirection = {0, 0, -1};
            transformComponent.yaw = 00.0f;
            transformComponent.yawRenderingOffset = 180.0f;
            transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
            transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));

            transformComponent.scale = glm::scale(glm::mat4(1.0f), {squadronLeaderScale, squadronLeaderScale, squadronLeaderScale});
            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Circle patrol 1";
            registry.emplace<TagComponent>(entity, tagComponent);

            LuaComponent luaComponent{};
            luaComponent.type = "AiPatrolCircle";
            luaComponent.path = "lua/ai_patrol_circle.out";
            registry.emplace<LuaComponent>(entity, luaComponent);

            MeshInfo meshInfo{};
            meshInfo.modelPath = "models/fighter/fighter.obj";
            meshInfo.textureAtlasPath = "models/fighter/" + fighterJetComponent.type + ".jpg";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.meshManager->getMesh(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
        // {
        //     entt::entity entity = registry.create();
        //
        //     FighterJetComponent fighterJetComponent{};
        //     fighterJetComponent.type = "krulspeld2";
        //     registry.emplace<FighterJetComponent>(entity, fighterJetComponent);
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = { squadronLeaderPosition.x + squadronLeaderScale, squadronLeaderPosition.y, squadronLeaderPosition.z + squadronLeaderScale};
        //     transformComponent.worldUpDirection = {0, 1, 0};
        //     transformComponent.forwardDirection = {0, 0, -1};
        //     transformComponent.yaw = 00.0f;
        //     transformComponent.yawRenderingOffset = 180.0f;
        //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        //
        //     transformComponent.scale = glm::scale(glm::mat4(1.0f), {squadronLeaderScale, squadronLeaderScale, squadronLeaderScale});
        //     registry.emplace<TransformComponent>(entity, transformComponent);
        //
        //     TagComponent tagComponent{};
        //     tagComponent.tag = "Circle patrol 2";
        //     registry.emplace<TagComponent>(entity, tagComponent);
        //
        //     LuaComponent luaComponent{};
        //     luaComponent.type = "AiPatrolCircle";
        //     luaComponent.path = "lua/ai_patrol_circle.out";
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
        //     fighterJetComponent.type = "krulspeld3";
        //     registry.emplace<FighterJetComponent>(entity, fighterJetComponent);
        //
        //     TransformComponent transformComponent{};
        //     transformComponent.position = { squadronLeaderPosition.x + squadronLeaderScale, squadronLeaderPosition.y, squadronLeaderPosition.z - squadronLeaderScale};
        //     transformComponent.worldUpDirection = {0, 1, 0};
        //     transformComponent.forwardDirection = {0, 0, -1};
        //     transformComponent.yaw = 00.0f;
        //     transformComponent.yawRenderingOffset = 180.0f;
        //     transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
        //     transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));
        //
        //     transformComponent.scale = glm::scale(glm::mat4(1.0f), {squadronLeaderScale, squadronLeaderScale, squadronLeaderScale});
        //     registry.emplace<TransformComponent>(entity, transformComponent);
        //
        //     TagComponent tagComponent{};
        //     tagComponent.tag = "Circle patrol 3";
        //     registry.emplace<TagComponent>(entity, tagComponent);
        //
        //     LuaComponent luaComponent{};
        //     luaComponent.type = "AiPatrolCircle";
        //     luaComponent.path = "lua/ai_patrol_circle.out";
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
        //--------------------------------------------------------------------------------------------------------------
        // Line patrol squadron
        //--------------------------------------------------------------------------------------------------------------
        {
            entt::entity entity = registry.create();

            FighterJetComponent fighterJetComponent{};
            fighterJetComponent.type = "krulspeld4";
            registry.emplace<FighterJetComponent>(entity, fighterJetComponent);

            TransformComponent transformComponent{};
            transformComponent.position = { -50, 50, -50 };
            transformComponent.worldUpDirection = {0, 1, 0};
            transformComponent.forwardDirection = {-1, 0, 0};
            transformComponent.yaw = -180.0f;
            transformComponent.yawRenderingOffset = 180.0f;
            transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
            transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));

            transformComponent.scale = glm::scale(glm::mat4(1.0f), {100, 100, 100});
            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Line patrol 1";
            registry.emplace<TagComponent>(entity, tagComponent);

            LuaComponent luaComponent{};
            luaComponent.type = "AiPatrolLine";
            luaComponent.path = "lua/ai_patrol_line.out";
            registry.emplace<LuaComponent>(entity, luaComponent);

            MeshInfo meshInfo{};
            meshInfo.modelPath = "models/fighter/fighter.obj";
            meshInfo.textureAtlasPath = "models/fighter/" + fighterJetComponent.type + ".jpg";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.meshManager->getMesh(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
        {
            entt::entity entity = registry.create();

            FighterJetComponent fighterJetComponent{};
            fighterJetComponent.type = "krulspeld5";
            registry.emplace<FighterJetComponent>(entity, fighterJetComponent);

            TransformComponent transformComponent{};
            transformComponent.position = { -50, 50, -150 };
            transformComponent.worldUpDirection = {0, 1, 0};
            transformComponent.forwardDirection = {-1, 0, 0};
            transformComponent.yaw = -180.0f;
            transformComponent.yawRenderingOffset = 180.0f;
            transformComponent.rightDirection = glm::normalize(glm::cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
            transformComponent.upDirection = glm::normalize(glm::cross(transformComponent.rightDirection, transformComponent.forwardDirection));

            transformComponent.scale = glm::scale(glm::mat4(1.0f), {100, 100, 100});
            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Line patrol 2";
            registry.emplace<TagComponent>(entity, tagComponent);

            LuaComponent luaComponent{};
            luaComponent.type = "AiPatrolLine";
            luaComponent.path = "lua/ai_patrol_line.out";
            registry.emplace<LuaComponent>(entity, luaComponent);

            MeshInfo meshInfo{};
            meshInfo.modelPath = "models/fighter/fighter.obj";
            meshInfo.textureAtlasPath = "models/fighter/" + fighterJetComponent.type + ".jpg";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.meshManager->getMesh(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
    }

    void Scene::destroyEntities() {
        registry.clear();
    }
}
