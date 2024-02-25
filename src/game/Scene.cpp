#include "pch.h"
#include "Scene.h"

namespace Blink {
    Scene::Scene(Keyboard* keyboard) : keyboard(keyboard), player(registry.create()) {
    }

    bool Scene::initialize() {
        registry.emplace<TransformComponent>(player);
        return true;
    }

    void Scene::terminate() const {
    }

    glm::mat4 Scene::update(double timestep) {
        auto& position = registry.get<TransformComponent>(player).position;

        static float moveSpeed = 5.0f;
        float velocity = moveSpeed * timestep;
        if (keyboard->isPressed(Key::H)) {
            position.x -= velocity;
        }
        if (keyboard->isPressed(Key::L)) {
            position.x += velocity;
        }
        if (keyboard->isPressed(Key::J)) {
            position.y -= velocity;
        }
        if (keyboard->isPressed(Key::K)) {
            position.y += velocity;
        }

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
