#version 450

layout(set = 1, binding = 0) uniform sampler2D textureSamplers[16];

layout(location = 0) in VertexOutput {
    vec3 color;
    vec2 textureCoordinate;
    uint textureIndex;
} vertexOutput;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = texture(textureSamplers[vertexOutput.textureIndex], vertexOutput.textureCoordinate);
    outColor = texture(textureSamplers[vertexOutput.textureIndex], vertexOutput.textureCoordinate) * vec4(vertexOutput.color, 1.0);
}