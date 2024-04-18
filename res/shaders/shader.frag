#version 450

layout(binding = 1) uniform sampler2D textureSampler;

layout(location = 0) in VertexOutput {
    vec3 color;
    vec2 textureCoordinate;
} vertexOutput;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(textureSampler, vertexOutput.textureCoordinate);
}