#include "pch.h"
#include "Scene.h"
#include "Components.h"

namespace Blink {
    Scene::Scene(
        Keyboard* keyboard,
        LuaEngine* luaEngine
    ) : keyboard(keyboard),
        luaEngine(luaEngine),
        player(registry.create()) {
    }

    bool Scene::initialize() {
        registry.emplace<TransformComponent>(player);
        registry.emplace<IdComponent>(player);

        std::string luaFilepath = "lua/player.out";
        std::string luaTableName = "Player";

        luaEngine->createEntityType(luaTableName);
        luaEngine->loadFile(luaFilepath);

        luaEngine->createEntityBinding(&registry);
        luaEngine->createKeyboardBinding(keyboard);

        registry.emplace<LuaComponent>(player, luaFilepath, luaTableName);
        return true;
    }

    void Scene::terminate() const {
    }

    glm::mat4 Scene::update(double timestep) {

        auto& luaComponent = registry.get<LuaComponent>(player);
        luaEngine->update(luaComponent.luaTableName, timestep, player);

        auto& [position] = registry.get<TransformComponent>(player);

        // static float moveSpeed = 5.0f;
        // float velocity = moveSpeed * timestep;
        // if (keyboard->isPressed(Key::Numpad_4)) {
        //     position.x -= velocity;
        // }
        // if (keyboard->isPressed(Key::Numpad_6)) {
        //     position.x += velocity;
        // }
        // if (keyboard->isPressed(Key::Numpad_5)) {
        //     position.y -= velocity;
        // }
        // if (keyboard->isPressed(Key::Numpad_8)) {
        //     position.y += velocity;
        // }

        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        // TRANSLATION
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);

        // ROTATION
        float angle = time * glm::radians(90.0f);
        constexpr glm::vec3 axis(0.0f, 1.0f, 0.0f);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, axis);

        // SCALE
        glm::mat4 scale = glm::mat4(1.0f);

        return translation * rotation * scale;
    }

    entt::entity Scene::createEntity() {
        return registry.create();
    }

    void Scene::destroyEntity(const entt::entity entity) {
        registry.destroy(entity);
    }
}
