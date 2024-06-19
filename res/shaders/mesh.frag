#version 450

// DescriptorSet 0: Per-frame
// DescriptorSet 1: Per-mesh

// Per-mesh sampler
// Array size must match descriptor count of associated descriptor set layout (VkDescriptorSetLayoutBinding#descriptorCount)
layout(set = 1, binding = 0) uniform sampler2D textureSamplers[16];

// Vertex data forwarded from vertex shader
layout(location = 0) in VertexData {
    vec3 color;
    vec2 textureCoordinate;
    uint textureIndex;
} vertexData;

// Final fragment color
layout(location = 0) out vec4 fragmentColor;

void main() {
    // Sample a color from one of the meshes textures, and multiply it by the color of the vertex.
    // If the vertex color is white, there is no change to the sampled texture color.
    fragmentColor = texture(textureSamplers[vertexData.textureIndex], vertexData.textureCoordinate) * vec4(vertexData.color, 1.0);
}