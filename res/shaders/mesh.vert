#version 450

// DescriptorSet 0: Per-frame
// DescriptorSet 1: Per-mesh

// Per-frame view and projection matrices
layout(set = 0, binding = 0) uniform UniformBufferData {
    mat4 view;
    mat4 projection;
} uniformBufferData;

// Per-mesh model matrix
layout(push_constant) uniform PushConstantData {
    mat4 model;
} pushConstantData;

// Vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 textureCoordinate;
layout(location = 3) in uint textureIndex;

// Vertex data to forward to fragment shader
layout(location = 0) out VertexData {
    vec3 color;
    vec2 textureCoordinate;
    uint textureIndex;
} vertexData;

void main() {
    gl_Position = uniformBufferData.projection * uniformBufferData.view * pushConstantData.model * vec4(position, 1.0);

    // Forward data to fragment shader
    vertexData.color = color;
    vertexData.textureCoordinate = textureCoordinate;
    vertexData.textureIndex = textureIndex;
}