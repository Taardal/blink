#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "window/KeyEvent.h"
#include "graphics/Model.h"

namespace Blink {
    Scene::Scene(const SceneConfig& config) : config(config), player(registry.create()) {
        initializeEntityComponents();
        config.luaEngine->createEntityBindings(&registry);
    }

    Scene::~Scene() {
        BL_LOG_DEBUG("DESTROYING SCENE");
        for (entt::entity entity : registry.view<ModelComponent>()) {
            const ModelComponent& modelComponent = registry.get<ModelComponent>(entity);
            config.renderer->destroyModel(modelComponent.model);
        }
        registry.clear();
    }

    void Scene::onEvent(Event& event) {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::F1) {
            config.luaEngine->reloadScripts(&registry);
            BL_LOG_INFO("Recreated entity bindings");
        }
    }

    void Scene::update(double timestep) {
        config.luaEngine->updateEntityBindings(&registry, timestep);
    }

    void Scene::render() {
        // TRANSLATION
        auto& transformComponent = registry.get<TransformComponent>(player);
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), transformComponent.position);

        // ROTATION
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        float angle = time * glm::radians(90.0f);
        constexpr glm::vec3 axis(0.0f, 0.0f, 1.0f);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, axis);

        // SCALE
        glm::mat4 scale = glm::mat4(1.0f);

        glm::mat4 modelMatrix = translation * rotation * scale;
        glm::mat4 viewMatrix = config.camera->getViewMatrix();
        glm::mat4 projectionMatrix = config.camera->getProjectionMatrix();

        Frame frame{};
        frame.view = &viewMatrix;
        frame.projection = &projectionMatrix;

        if (!config.renderer->beginFrame(frame)) {
            return;
        }

        auto& modelComponent = registry.get<ModelComponent>(player);
        Model& model = modelComponent.model;

        model.modelMatrix = modelMatrix;
        config.renderer->render(frame, model);

        model.modelMatrix = glm::translate(glm::mat4(1.0f), transformComponent.position + 1.0f);
        config.renderer->render(frame, model);

        config.renderer->endFrame();
    }

    void Scene::initializeEntityComponents() {
        TransformComponent transformComponent{};
        transformComponent.position = { 2.0f, 2.0f, 2.0f };
        registry.emplace<TransformComponent>(player, transformComponent);

        TagComponent tagComponent{};
        tagComponent.tag = "Player";
        registry.emplace<TagComponent>(player, tagComponent);

        LuaComponent luaComponent{};
        luaComponent.type = "Player";
        luaComponent.filepath = "lua/player.out";
        registry.emplace<LuaComponent>(player, luaComponent);

        ModelComponent modelComponent{};
        modelComponent.model = config.renderer->createModel();
        registry.emplace<ModelComponent>(player, modelComponent);
    }
}
