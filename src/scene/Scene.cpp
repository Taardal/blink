#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "window/KeyEvent.h"
#include "graphics/Mesh.h"
#include "graphics/ViewProjection.h"

namespace Blink {
    Scene::Scene(const SceneConfig& config) : config(config), player(registry.create()), enemy(registry.create()) {
        initializePlayerComponents();
        //initializeEnemyComponents();
        config.luaEngine->createEntityBindings(&registry);
        //config.camera->setPosition({-2, 0, 0});
    }

    Scene::~Scene() {
        for (const entt::entity entity : registry.view<MeshComponent>()) {
            const MeshComponent& meshComponent = registry.get<MeshComponent>(entity);
            config.renderer->destroyMesh(meshComponent.mesh);
        }
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
            meshComponent.mesh.model = transformComponent.translation * transformComponent.rotation * transformComponent.scale;
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
        //transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //transformComponent.scale = glm::scale(transformComponent.scale, { 1, 1, 1 });
        registry.emplace<TransformComponent>(player, transformComponent);

        TagComponent tagComponent{};
        tagComponent.tag = "Player";
        registry.emplace<TagComponent>(player, tagComponent);

        LuaComponent luaComponent{};
        luaComponent.type = "Player";
        luaComponent.filepath = "lua/player.out";
        registry.emplace<LuaComponent>(player, luaComponent);

        MeshComponent meshComponent{};
        meshComponent.mesh = config.renderer->createMesh();
        registry.emplace<MeshComponent>(player, meshComponent);
    }

    void Scene::initializeEnemyComponents() {
        for (uint32_t i = 0; i < 5; i++) {
            entt::entity enemy = registry.create();

            TransformComponent transformComponent{};
            transformComponent.position = { i, i, i };
            transformComponent.translation = glm::translate(glm::mat4(1.0f), transformComponent.position);
            transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            transformComponent.rotation = glm::rotate(transformComponent.rotation, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            registry.emplace<TransformComponent>(enemy, transformComponent);

            TagComponent tagComponent{};
            tagComponent.tag = "Enemy" + std::to_string(i);
            registry.emplace<TagComponent>(enemy, tagComponent);

            MeshComponent meshComponent{};
            meshComponent.mesh = config.renderer->createMesh();
            registry.emplace<MeshComponent>(enemy, meshComponent);
        }
    }
}
