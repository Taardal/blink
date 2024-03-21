#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "window/KeyEvent.h"

namespace Blink {
    Scene::Scene(
        Keyboard* keyboard,
        LuaEngine* luaEngine
    ) : keyboard(keyboard),
        luaEngine(luaEngine),
        player(registry.create())
    {
        initializeEntityComponents();
        luaEngine->createEntityBindings(&registry);
    }

    Scene::~Scene() {
        registry.clear();
    }

    glm::mat4 Scene::update(double timestep) {
        luaEngine->updateEntityBindings(&registry, timestep);

        // TRANSLATION
        auto& transformComponent = registry.get<TransformComponent>(player);
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), transformComponent.position);

        // ROTATION
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        float angle = time * glm::radians(90.0f);
        constexpr glm::vec3 axis(0.0f, 1.0f, 0.0f);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, axis);

        // SCALE
        glm::mat4 scale = glm::mat4(1.0f);

        return translation * rotation * scale;
    }

    void Scene::onEvent(Event& event) {
        if (event.type != EventType::KeyPressed) {
            return;
        }
        if (event.as<KeyPressedEvent>().key == Key::R) {
            luaEngine->reload(&registry);
            BL_LOG_INFO("Recreated entity bindings");
        }
    }

    void Scene::initializeEntityComponents() {
        TransformComponent transformComponent{};
        transformComponent.position = { 0.0f, 0.0f, 0.0f };
        registry.emplace<TransformComponent>(player, transformComponent);

        TagComponent tagComponent{};
        tagComponent.tag = "Player";
        registry.emplace<TagComponent>(player, tagComponent);

        LuaComponent luaComponent{};
        luaComponent.type = "Player";
        luaComponent.filepath = "lua/player.out";
        registry.emplace<LuaComponent>(player, luaComponent);
    }
}
