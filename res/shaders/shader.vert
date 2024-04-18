#version 450

layout(binding = 0) uniform UniformBufferData {
    mat4 view;
    mat4 projection;
} uniformBufferData;

layout(push_constant) uniform PushConstantData {
    mat4 model;
} pushConstantData;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 textureCoordinate;

layout(location = 0) out VertexOutput {
    vec3 color;
    vec2 textureCoordinate;
} vertexOutput;

void main() {
    gl_Position = uniformBufferData.projection * uniformBufferData.view * pushConstantData.model * vec4(position, 1.0);
    vertexOutput.color = color;
    vertexOutput.textureCoordinate = textureCoordinate;
}