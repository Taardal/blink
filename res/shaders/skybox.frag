#version 450

// DescriptorSet 0: Per-frame
// DescriptorSet 1: Per-skybox

// Per-skybox sampler
layout(set = 1, binding = 0) uniform samplerCube textureSampler;

// Vertex data forwarded from vertex shader
layout(location = 0) in VertexData {
    vec3 textureCoordinate;
} vertexData;

// Final fragment color
layout(location = 0) out vec4 fragmentColor;

void main() {
    fragmentColor = texture(textureSampler, vertexData.textureCoordinate);
}
