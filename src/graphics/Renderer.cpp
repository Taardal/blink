#include "pch.h"
#include "Renderer.h"
#include "graphics/UniformBufferData.h"
#include "graphics/VulkanImage.h"
#include "graphics/PushConstantData.h"
#include "window/KeyEvent.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Blink {
    Renderer::Renderer(const RendererConfig& config) {
        VulkanAppConfig vulkanAppConfig{};
        vulkanAppConfig.window = config.window;
        vulkanAppConfig.applicationName = config.applicationName;
        vulkanAppConfig.engineName = config.engineName;
        vulkanAppConfig.validationLayersEnabled = true;
        vulkanApp = new VulkanApp(vulkanAppConfig);

        VulkanPhysicalDeviceConfig physicalDeviceConfig{};
        physicalDeviceConfig.vulkanApp = vulkanApp;
        physicalDevice = new VulkanPhysicalDevice(physicalDeviceConfig);

        VulkanDeviceConfig deviceConfig{};
        deviceConfig.physicalDevice = physicalDevice;
        device = new VulkanDevice(deviceConfig);

        VulkanCommandPoolConfig commandPoolConfig{};
        commandPoolConfig.device = device;
        commandPool = new VulkanCommandPool(commandPoolConfig);

        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        BL_ASSERT_THROW_VK_SUCCESS(commandPool->allocateCommandBuffers(&commandBuffers));

        VulkanSwapChainConfig swapChainConfig{};
        swapChainConfig.window = config.window;
        swapChainConfig.vulkanApp = vulkanApp;
        swapChainConfig.device = device;
        swapChainConfig.commandPool = commandPool;
        swapChain = new VulkanSwapChain(swapChainConfig);

        createTextureSampler();
        createDescriptorSetLayout();
        createDescriptorPool();
        createGraphicsPipelineObjects();
    }

    Renderer::~Renderer() {
        destroyGraphicsPipelineObjects();

        device->destroyDescriptorPool(descriptorPool);
        device->destroyDescriptorSetLayout(descriptorSetLayout);

        device->destroySampler(textureSampler);

        delete swapChain;
        delete commandPool;
        delete device;
        delete physicalDevice;
        delete vulkanApp;
    }

    void Renderer::waitUntilIdle() const {
        BL_ASSERT_THROW_VK_SUCCESS(device->waitUntilIdle());
    }

    void Renderer::onEvent(Event& event) {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::F2) {
            reloadShaders();
            return;
        }
        swapChain->onEvent(event);
    }

    bool Renderer::beginFrame() {
        if (!swapChain->beginFrame(currentFrame)) {
            return false;
        }
        currentCommandBuffer = commandBuffers[currentFrame];
        BL_ASSERT_THROW_VK_SUCCESS(currentCommandBuffer.begin());
        swapChain->beginRenderPass(currentCommandBuffer);
        graphicsPipeline->bind(currentCommandBuffer);
        return true;
    }

    void Renderer::renderMesh(const Mesh& mesh, const ViewProjection& viewProjection) const {
        setMeshUniformData(mesh, viewProjection);
        setMeshPushConstantData(mesh);
        bindMesh(mesh);
        drawMeshIndexed(mesh);
    }

    void Renderer::endFrame() {
        swapChain->endRenderPass(currentCommandBuffer);
        BL_ASSERT_THROW_VK_SUCCESS(currentCommandBuffer.end());
        swapChain->endFrame(currentCommandBuffer);
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    Mesh Renderer::createMesh() const {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        Image image = config.fileSystem->readImage("textures/viking_room.png");

        VulkanImageConfig textureImageConfig{};
        textureImageConfig.device = device;
        textureImageConfig.commandPool = commandPool;
        textureImageConfig.width = image.width;
        textureImageConfig.height = image.height;
        textureImageConfig.format = VK_FORMAT_R8G8B8A8_SRGB;
        textureImageConfig.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureImageConfig.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
        textureImageConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        auto texture = new VulkanImage(textureImageConfig);
        texture->setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        texture->setData(image);
        texture->setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        image.free();

        /*
         * An OBJ file consists of positions, normals, texture coordinates and faces.
         * Faces consist of an arbitrary amount of vertices, where each vertex refers to a position, normal and/or texture coordinate by index.
         * The attrib container holds all of the positions, normals and texture coordinates in its attrib.vertices, attrib.normals and attrib.texcoords vectors.
         * The shapes container contains all of the separate objects and their faces.
         * Each face consists of an array of vertices, and each vertex contains the indices of the position, normal and texture coordinate attributes.
         */
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        const std::string modelFilepath = "models/viking_room.obj";
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelFilepath.c_str())) {
            BL_THROW(err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const tinyobj::shape_t& shape : shapes) {
            for (const tinyobj::index_t& index : shape.mesh.indices) {

                Vertex vertex{};
                vertex.color = {1.0f, 1.0f, 1.0f};

                // Unfortunately the attrib.vertices array is an array of float values instead of something like glm::vec3, so we need to multiply the index by 3.
                vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                };
                // Similarly, there are two texture coordinate components per entry.
                vertex.textureCoordinate = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        // The OBJ format assumes a coordinate system where a vertical coordinate of 0 means the bottom of the image,
                        // however we've uploaded our image into Vulkan in a top to bottom orientation where 0 means the top of the image.
                        // This can be solved by flipping the vertical component of the texture coordinates
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };
                // The offsets of 0, 1 and 2 are used to access the X, Y and Z components,
                // or the U and V components in the case of texture coordinates.

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = (uint32_t) vertices.size();
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        }

        VulkanVertexBufferConfig vertexBufferConfig{};
        vertexBufferConfig.device = device;
        vertexBufferConfig.commandPool = commandPool;
        vertexBufferConfig.size = sizeof(vertices[0]) * vertices.size();
        auto vertexBuffer = new VulkanVertexBuffer(vertexBufferConfig);
        vertexBuffer->setData(vertices);

        VulkanIndexBufferConfig indexBufferConfig{};
        indexBufferConfig.device = device;
        indexBufferConfig.commandPool = commandPool;
        indexBufferConfig.size = sizeof(indices[0]) * indices.size();
        auto indexBuffer = new VulkanIndexBuffer(indexBufferConfig);
        indexBuffer->setData(indices);

        VulkanUniformBufferConfig uniformBufferConfig{};
        uniformBufferConfig.device = device;
        uniformBufferConfig.commandPool = commandPool;
        uniformBufferConfig.size = sizeof(UniformBufferData);
        auto uniformBuffer = new VulkanUniformBuffer(uniformBufferConfig);

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

        BL_ASSERT_THROW_VK_SUCCESS(device->allocateDescriptorSets(&descriptorSetAllocateInfo, &descriptorSet));

        VkDescriptorBufferInfo descriptorBufferInfo{};
        descriptorBufferInfo.buffer = *uniformBuffer;
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = sizeof(UniformBufferData);

        VkDescriptorImageInfo descriptorImageInfo{};
        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptorImageInfo.imageView = texture->getImageView();
        descriptorImageInfo.sampler = textureSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSet;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &descriptorBufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSet;
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &descriptorImageInfo;

        device->updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data());

        Mesh mesh{};
        mesh.vertices = vertices;
        mesh.indices = indices;
        mesh.vertexBuffer = vertexBuffer;
        mesh.indexBuffer = indexBuffer;
        mesh.uniformBuffer = uniformBuffer;
        mesh.texture = texture;
        mesh.descriptorSet = descriptorSet;
        return mesh;
    }

    void Renderer::destroyMesh(const Mesh& mesh) const {
        delete mesh.texture;
        delete mesh.uniformBuffer;
        delete mesh.indexBuffer;
        delete mesh.vertexBuffer;
    }

    void Renderer::setMeshUniformData(const Mesh& mesh, const ViewProjection& viewProjection) const {
        UniformBufferData uniformBufferData{};
        uniformBufferData.view = viewProjection.view;
        uniformBufferData.projection = viewProjection.projection;

        // GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted.
        // The easiest way to compensate for that is to flip the sign on the scaling factor of the Y rotationAxis in the projection matrix.
        // If we don't do this, then the image will be rendered upside down.
        //
        // This change causes the vertices to be drawn in counter-clockwise order instead of clockwise order.
        // This causes backface culling to kick in and prevents any geometry from being drawn.
        // To fix this the graphics pipeline's rasterization state should have a counter clockwise front-facing triangle orientation to be used for culling.
        //
        // VkPipelineRasterizationStateCreateInfo rasterizationState{};
        // rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
        // rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        uniformBufferData.projection[1][1] *= -1;

        mesh.uniformBuffer->setData(&uniformBufferData);
    }

    void Renderer::setMeshPushConstantData(const Mesh& mesh) const {
        PushConstantData pushConstantData{};
        pushConstantData.model = mesh.model;
        constexpr uint32_t offset = 0;
        vkCmdPushConstants(
            currentCommandBuffer,
            graphicsPipeline->getLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            offset,
            sizeof(PushConstantData),
            &pushConstantData
        );
    }

    void Renderer::bindMesh(const Mesh& mesh) const {
        constexpr uint32_t firstSet = 0;
        constexpr uint32_t descriptorSetCount = 1;
        constexpr uint32_t dynamicOffsetCount = 0;
        constexpr uint32_t* dynamicOffsets = nullptr;
        vkCmdBindDescriptorSets(
            currentCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphicsPipeline->getLayout(),
            firstSet,
            descriptorSetCount,
            &mesh.descriptorSet,
            dynamicOffsetCount,
            dynamicOffsets
        );

        mesh.vertexBuffer->bind(currentCommandBuffer);
        mesh.indexBuffer->bind(currentCommandBuffer);
    }

    void Renderer::drawMeshIndexed(const Mesh& mesh) const {
        constexpr uint32_t instanceCount = 1;
        constexpr uint32_t firstIndex = 0;
        constexpr uint32_t vertexOffset = 0;
        constexpr uint32_t firstInstance = 0;
        vkCmdDrawIndexed(
            currentCommandBuffer,
            (uint32_t) mesh.indices.size(),
            instanceCount,
            firstIndex,
            vertexOffset,
            firstInstance
        );
    }

    void Renderer::reloadShaders() {
        BL_ASSERT_THROW_VK_SUCCESS(device->waitUntilIdle());
        compileShaders();
        destroyGraphicsPipelineObjects();
        createGraphicsPipelineObjects();
        BL_LOG_INFO("Reloaded shaders");
    }

    void Renderer::compileShaders() const {
        std::stringstream ss;
        ss << "cmake";
        ss << " -D SHADERS_SOURCE_DIR=" << CMAKE_SHADERS_SOURCE_DIR;
        ss << " -D SHADERS_OUTPUT_DIR=" << CMAKE_SHADERS_OUTPUT_DIR;
        ss << " -P " << CMAKE_SCRIPTS_DIR << "/compile_shaders.cmake";
        std::string command = ss.str();
        std::system(command.c_str());
    }

    void Renderer::createTextureSampler() {
        const VkPhysicalDeviceProperties& physicalDeviceProperties = physicalDevice->getProperties();

        VkSamplerCreateInfo textureSamplerCreateInfo{};
        textureSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        textureSamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        textureSamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        textureSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        textureSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        textureSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        textureSamplerCreateInfo.anisotropyEnable = VK_TRUE;
        textureSamplerCreateInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;
        textureSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        textureSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        textureSamplerCreateInfo.compareEnable = VK_FALSE;
        textureSamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        textureSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        textureSamplerCreateInfo.mipLodBias = 0.0f;
        textureSamplerCreateInfo.minLod = 0.0f;
        textureSamplerCreateInfo.maxLod = 0.0f;

        BL_ASSERT_THROW_VK_SUCCESS(device->createSampler(&textureSamplerCreateInfo, &textureSampler));
    }

    void Renderer::createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
            uboLayoutBinding,
            samplerLayoutBinding
        };
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = (uint32_t) bindings.size();
        layoutInfo.pBindings = bindings.data();

        BL_ASSERT_THROW_VK_SUCCESS(device->createDescriptorSetLayout(&layoutInfo, &descriptorSetLayout));
    }

    void Renderer::createDescriptorPool() {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = (uint32_t) MAX_FRAMES_IN_FLIGHT;
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = (uint32_t) MAX_FRAMES_IN_FLIGHT;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = (uint32_t) poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = (uint32_t) MAX_FRAMES_IN_FLIGHT;

        BL_ASSERT_THROW_VK_SUCCESS(device->createDescriptorPool(&poolInfo, &descriptorPool));
    }

    void Renderer::createGraphicsPipelineObjects() {
        VulkanShaderConfig vertexShaderConfig{};
        vertexShaderConfig.device = device;
        vertexShaderConfig.bytes = config.fileSystem->readBytes("shaders/shader.vert.spv");
        vertexShader = new VulkanShader(vertexShaderConfig);

        VulkanShaderConfig fragmentShaderConfig{};
        fragmentShaderConfig.device = device;
        fragmentShaderConfig.bytes = config.fileSystem->readBytes("shaders/shader.frag.spv");
        fragmentShader = new VulkanShader(fragmentShaderConfig);

        VulkanGraphicsPipelineConfig graphicsPipelineConfig{};
        graphicsPipelineConfig.device = device;
        graphicsPipelineConfig.swapChain = swapChain;
        graphicsPipelineConfig.vertexShader = vertexShader;
        graphicsPipelineConfig.fragmentShader = fragmentShader;
        graphicsPipelineConfig.descriptorSetLayout = descriptorSetLayout;
        graphicsPipeline = new VulkanGraphicsPipeline(graphicsPipelineConfig);
    }

    void Renderer::destroyGraphicsPipelineObjects() const {
        delete graphicsPipeline;
        delete fragmentShader;
        delete vertexShader;
    }

}

