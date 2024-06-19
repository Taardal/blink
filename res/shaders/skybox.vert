#version 450

// DescriptorSet 0: Per-frame
// DescriptorSet 1: Per-mesh

// Per-frame view and projection matrices
layout(set = 0, binding = 0) uniform UniformBufferData {
    mat4 view;
    mat4 projection;
} uniformBufferData;

// Vertex attributes
layout(location = 0) in vec3 position;

// Vertex data to forward to fragment shader
layout(location = 0) out VertexData {
    vec3 textureCoordinate;
} vertexData;

void main() {

    //
    // Remove the translation from the view matrix
    //
    // We want the skybox to be centered around the player so that no matter how far the player moves,
    // the skybox won't get any closer, giving the impression the surrounding environment is extremely large.
    //
    // The view matrix however transforms all the skybox's positions by rotating, scaling and translating them,
    // so if the player moves, the cubemap moves as well.
    //
    // We want to remove the translation part of the view matrix so only rotation will affect the skybox's
    // position vectors.
    //
    // We can remove the translation section of transformation matrices by taking the upper-left 3x3 matrix
    // of the 4x4 matrix
    //
    mat4 viewWithoutTranslation = mat4(mat3(uniformBufferData.view));

    // Final vertex position
    gl_Position = uniformBufferData.projection * viewWithoutTranslation * vec4(position.xyz, 1.0);

    //
    // Forward data to fragment shader
    //
    // A cubemap texture uses a 3D vector to look up the color value from six 2D textures that form the faces of a cube.
    // This vector effectively represents a direction in 3D space, and the cubemap texture function (texture(skybox, TexCoords))
    // uses this vector to sample the correct face of the cubemap and the appropriate texel within that face.
    //
    // The local position of each vertex of the cube is essentially a direction vector from the center of the cube
    // (and by extension the camera) to the vertex.
    //
    // This direction vector is exactly what is needed to sample a cubemap texture.
    //
    vertexData.textureCoordinate = position;
}
