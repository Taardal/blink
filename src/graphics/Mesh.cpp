#include "Mesh.h"

namespace Blink {
    VkVertexInputBindingDescription MeshVertex::getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(MeshVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    std::vector<VkVertexInputAttributeDescription> MeshVertex::getAttributeDescriptions() {
        return {
            {
                .binding = 0,
                .location = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(MeshVertex, position),
            },
            {
                .binding = 0,
                .location = 1,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(MeshVertex, color),
            },
            {
                .binding = 0,
                .location = 2,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(MeshVertex, textureCoordinate),
            },
            {
                .binding = 0,
                .location = 3,
                .format = VK_FORMAT_R32_UINT,
                .offset = offsetof(MeshVertex, textureIndex),
            }
        };
    }
}

//
// When loading a mesh from a file, there is a lot of duplicated vertex data because many vertices are included
// in multiple triangles.
//
// As a performance improvement we want to only use the unique vertices and use the index buffer to reuse them
// whenever they come up.
//
// One way to do this is to use a map to keep track of the unique vertices and respective indices,
// but for this to work we need to be able to use a vertex as the key of the map.
//
// We achieve this by implementing a custom hashing function and an equality operator function:
// - https://en.cppreference.com/w/cpp/utility/hash
//
bool Blink::MeshVertex::operator==(const MeshVertex& other) const {
    return position == other.position
           && color == other.color
           && textureCoordinate == other.textureCoordinate
           && textureIndex == other.textureIndex;
}
size_t std::hash<Blink::MeshVertex>::operator()(Blink::MeshVertex const& vertex) const noexcept {
    return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
               (hash<glm::vec2>()(vertex.textureCoordinate) << 1) ^
               (hash<uint32_t>()(vertex.textureIndex) << 1);
}
