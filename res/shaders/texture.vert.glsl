#version 410

struct Vertex {
    vec4 color;
    vec2 textureCoordinate;
    float textureIndex;
    float tilingFactor;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 textureCoordinate;
layout(location = 3) in float textureIndex;
layout(location = 4) in float tilingFactor;

out Vertex vertex;

//uniform mat4 viewProjection;

void main() {
    vertex = Vertex(color, textureCoordinate, textureIndex, tilingFactor);
    //gl_Position = viewProjection * vec4(position, 1.0);
    gl_Position = vec4(position, 1.0);
}