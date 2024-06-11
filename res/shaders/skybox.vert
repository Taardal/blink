#version 450

layout(binding = 0) uniform UBO {
    mat4 view;
    mat4 projection;
} ubo;

layout(location = 0) in vec3 inPos;

layout(location = 0) out vec3 outUVW;

void main()
{
    outUVW = inPos;

    // Convert cubemap coordinates into Vulkan coordinate space
    //outUVW.xy *= -1.0;

    // Remove translation from view matrix
    mat4 viewMat = mat4(mat3(ubo.view));

    gl_Position = ubo.projection * viewMat * vec4(inPos.xyz, 1.0);
}
