#version 410

struct Vertex {
    vec4 color;
    vec2 textureCoordinate;
    float textureIndex;
    float tilingFactor;
};

layout(location = 0) out vec4 fragmentColor;

in Vertex vertex;

uniform sampler2D textureSamplers[16];

void main() {
    fragmentColor = texture(textureSamplers[0], vertex.textureCoordinate) * vertex.color;
}