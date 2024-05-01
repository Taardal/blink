#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "window/KeyEvent.h"
#include "graphics/Mesh.h"
#include "graphics/ViewProjection.h"

namespace Blink {
    Scene::Scene(const SceneConfig& config) : config(config), player(registry.create()), enemy(registry.create()) {
        initializePlayerComponents();
        initializeEnemyComponents();
        config.luaEngine->createEntityBindings(&registry);
        //config.camera->setPosition({-2, 0, 0});
    }

    Scene::~Scene() {
        // for (const entt::entity entity : registry.view<MeshComponent>()) {
        //     registry.get<MeshComponent>(entity).mesh = nullptr;
        // }
        registry.clear();
    }

    void Scene::onEvent(Event& event) {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::F1) {
            config.luaEngine->reloadScripts(&registry);
        }
    }

    void Scene::update(double timestep) {
        config.luaEngine->updateEntityBindings(&registry, timestep);
        for (const entt::entity entity : registry.view<TransformComponent, MeshComponent>()) {
            auto& transformComponent = registry.get<TransformComponent>(entity);
            auto& meshComponent = registry.get<MeshComponent>(entity);
            meshComponent.mesh->model = transformComponent.translation * transformComponent.rotation * transformComponent.scale;
        }
    }

    void Scene::render() {
        ViewProjection viewProjection{};
        viewProjection.view = config.camera->getView();
        viewProjection.projection = config.camera->getProjection();
        for (const entt::entity entity : registry.view<MeshComponent>()) {
            auto& meshComponent = registry.get<MeshComponent>(entity);
            config.renderer->renderMesh(meshComponent.mesh, viewProjection);
        }
    }

    void Scene::initializePlayerComponents() {
        TransformComponent transformComponent{};
        transformComponent.position = { 0, 0, 0 };
        transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
        registry.emplace<TransformComponent>(player, transformComponent);

        TagComponent tagComponent{};
        tagComponent.tag = "Player";
        registry.emplace<TagComponent>(player, tagComponent);

        LuaComponent luaComponent{};
        luaComponent.type = "Player";
        luaComponent.filepath = "lua/player.out";
        registry.emplace<LuaComponent>(player, luaComponent);

        MeshInfo meshInfo{};
        meshInfo.modelPath = "models/fighter/fighter.obj";
        meshInfo.textureAtlasPath = "models/fighter/crono782.jpg";

        MeshComponent meshComponent{};
        meshComponent.mesh = config.resourceLoader->loadModel(meshInfo);
        registry.emplace<MeshComponent>(player, meshComponent);
    }

    void Scene::initializeEnemyComponents() {
        {
            entt::entity entity = registry.create();

            TransformComponent transformComponent{};
            transformComponent.position = { 0, 0, 1 };
            transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
            transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Viking Room";
            registry.emplace<TagComponent>(entity, tagComponent);

            MeshInfo meshInfo{};
            meshInfo.modelPath = "models/viking_room/viking_room.obj";
            meshInfo.textureAtlasPath = "models/viking_room/viking_room.png";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.resourceLoader->loadModel(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
        {
            entt::entity entity = registry.create();

            TransformComponent transformComponent{};
            transformComponent.position = { 0, 0, 0 };
            transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
            registry.emplace<TransformComponent>(entity, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Sibenik";
            registry.emplace<TagComponent>(entity, tagComponent);

            MeshInfo meshInfo{};
            meshInfo.modelPath = "models/sibenik/sibenik.obj";
            meshInfo.texturesDirectoryPath = "models/sibenik";

            MeshComponent meshComponent{};
            meshComponent.mesh = config.resourceLoader->loadModel(meshInfo);
            registry.emplace<MeshComponent>(entity, meshComponent);
        }
    }
}
