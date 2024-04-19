#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/glm.hpp>

namespace Blink {

    typedef std::array<VkVertexInputAttributeDescription, 3> VertexAttributeDescriptions;

    struct Vertex {
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 color = { 1.0f, 1.0f, 1.0f };
        glm::vec2 textureCoordinate = { 0.0f, 0.0f };

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static VertexAttributeDescriptions getAttributeDescriptions() {
            VertexAttributeDescriptions attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, textureCoordinate);

            return attributeDescriptions;
        }
        bool operator==(const Vertex& other) const {
            return position == other.position && color == other.color && textureCoordinate == other.textureCoordinate;
        }
    };
}

// A hash function for Vertex is implemented by specifying a template specialization for std::hash<T>.
// Hash functions are a complex topic, but cppreference.com recommends the following approach combining the fields of a struct to create a decent quality hash function
//
// Source:
//  - http://en.cppreference.com/w/cpp/utility/hash
namespace std {
    template<>
    struct hash<Blink::Vertex> {
        size_t operator()(Blink::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.textureCoordinate) << 1);
        }
    };
}