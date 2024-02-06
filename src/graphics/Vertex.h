#pragma once

#include <glm/glm.hpp>

namespace Blink {

    struct Vertex {
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 color = { 1.0f, 1.0f, 1.0f };

        //glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        //glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        //glm::vec2 textureCoordinate = { 0.0f, 0.0f };
        //float textureIndex = 0.0f;
        //float tilingFactor = 0.0f;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            return attributeDescriptions;
        }
    };
}
