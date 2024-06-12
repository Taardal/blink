#version 450

// Per mesh descriptor set
// Array size must match descriptor count of associated descriptor set layout (VkDescriptorSetLayoutBinding#descriptorCount)
layout(set = 1, binding = 0) uniform sampler2D textureSamplers[16];

// Vertex data forwarded from vertex shader
layout(location = 0) in VertexData {
    vec3 color;
    vec2 textureCoordinate;
    uint textureIndex;
} vertexData;

layout(location = 0) out vec4 fragmentColor;

void main() {
    fragmentColor = texture(textureSamplers[vertexData.textureIndex], vertexData.textureCoordinate) * vec4(vertexData.color, 1.0);
}