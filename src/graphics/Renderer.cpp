#include "pch.h"
#include "Renderer.h"
#include "graphics/UniformBufferData.h"
#include "graphics/VulkanImage.h"
#include "graphics/PushConstantData.h"
#include "window/KeyEvent.h"

namespace Blink {
    Renderer::Renderer(const RendererConfig& config) : config(config) {
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

        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for (uint32_t i = 0; i < uniformBuffers.size(); i++) {
            VulkanUniformBufferConfig uniformBufferConfig{};
            uniformBufferConfig.device = device;
            uniformBufferConfig.commandPool = commandPool;
            uniformBufferConfig.size = sizeof(UniformBufferData);
            uniformBuffers[i] = new VulkanUniformBuffer(uniformBufferConfig);
        }

        createTextureSampler();
        createDescriptorSetLayouts();
        createDescriptorPools();
        createDescriptorSets();
        createGraphicsPipelineObjects();
    }

    Renderer::~Renderer() {
        destroyGraphicsPipelineObjects();

        device->destroyDescriptorPool(perMeshDescriptorPool);
        device->destroyDescriptorSetLayout(perMeshDescriptorSetLayout);

        device->destroyDescriptorPool(perFrameDescriptorPool);
        device->destroyDescriptorSetLayout(perFrameDescriptorSetLayout);

        device->destroySampler(textureSampler);

        for (VulkanUniformBuffer* uniformBuffer : uniformBuffers) {
            delete uniformBuffer;
        }
        uniformBuffers.clear();

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
        setUniformData(viewProjection);
        setPushConstantData(mesh);
        bindMesh(mesh);
        drawMeshIndexed(mesh);
    }

    void Renderer::endFrame() {
        swapChain->endRenderPass(currentCommandBuffer);
        BL_ASSERT_THROW_VK_SUCCESS(currentCommandBuffer.end());
        swapChain->endFrame(currentCommandBuffer);
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    Mesh Renderer::createMesh(const MeshConfig& meshConfig) const {
        std::string path = "models/viking_room/viking_room.png";
        std::shared_ptr<Image> placeholderImage = config.resourceLoader->loadTexture(path);

        std::shared_ptr<Model> model = config.resourceLoader->loadModel(meshConfig);

        std::shared_ptr<Image> textureAtlasImage = nullptr;
        if (!meshConfig.textureAtlasPath.empty()) {
            textureAtlasImage = config.resourceLoader->loadTexture(meshConfig.textureAtlasPath);
        }

        std::vector<VulkanImage*> textures;
        for (uint32_t i = 0; i < MAX_DESCRIPTORS_PER_STAGE; ++i) {
            std::shared_ptr<Image> image;

            if (textureAtlasImage != nullptr) {
                image = textureAtlasImage;
            } else if (i > model->materials.size() - 1) {
                image = placeholderImage;
            } else {
                tinyobj::material_t& material = model->materials[i];
                if (material.diffuse_texname.empty()) {
                    image = placeholderImage;
                } else {
                    image = config.resourceLoader->loadTexture(meshConfig.texturesDirectoryPath + "/" + material.diffuse_texname);
                }
            }

            auto texture = new VulkanImage({
                .device = device,
                .commandPool = commandPool,
                .width = (uint32_t) image->width,
                .height = (uint32_t) image->height,
                .format = VK_FORMAT_R8G8B8A8_SRGB,
                .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                .aspect = VK_IMAGE_ASPECT_COLOR_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            });
            texture->setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            texture->setData(image);
            texture->setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            textures.push_back(texture);
        }

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = perMeshDescriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &perMeshDescriptorSetLayout;

        BL_ASSERT_THROW_VK_SUCCESS(device->allocateDescriptorSets(&descriptorSetAllocateInfo, &descriptorSet));

        for (uint32_t i = 0; i < textures.size(); ++i) {
            VulkanImage* texture = textures[i];

            VkDescriptorImageInfo descriptorImageInfo{};
            descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptorImageInfo.imageView = texture->getImageView();
            descriptorImageInfo.sampler = textureSampler;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSet;
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = i;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &descriptorImageInfo;

            device->updateDescriptorSets(1, &descriptorWrite);
        }

        VulkanVertexBufferConfig vertexBufferConfig{};
        vertexBufferConfig.device = device;
        vertexBufferConfig.commandPool = commandPool;
        vertexBufferConfig.size = sizeof(model->vertices[0]) * model->vertices.size();
        auto vertexBuffer = new VulkanVertexBuffer(vertexBufferConfig);
        vertexBuffer->setData(model->vertices);

        VulkanIndexBufferConfig indexBufferConfig{};
        indexBufferConfig.device = device;
        indexBufferConfig.commandPool = commandPool;
        indexBufferConfig.size = sizeof(model->indices[0]) * model->indices.size();
        auto indexBuffer = new VulkanIndexBuffer(indexBufferConfig);
        indexBuffer->setData(model->indices);

        Mesh mesh{};
        mesh.textures = textures;
        mesh.descriptorSet = descriptorSet;
        mesh.vertexBuffer = vertexBuffer;
        mesh.indexBuffer = indexBuffer;
        mesh.vertices = model->vertices;
        mesh.indices = model->indices;
        return mesh;
    }

    void Renderer::destroyMesh(const Mesh& mesh) const {
        for (auto texture : mesh.textures) {
            delete texture;
        }
        delete mesh.texture;
        delete mesh.indexBuffer;
        delete mesh.vertexBuffer;
    }

    void Renderer::setUniformData(const ViewProjection& viewProjection) const {
        VulkanUniformBuffer* uniformBuffer = uniformBuffers[currentFrame];

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

        uniformBuffer->setData(&uniformBufferData);
    }

    void Renderer::setPushConstantData(const Mesh& mesh) const {
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
        mesh.vertexBuffer->bind(currentCommandBuffer);
        mesh.indexBuffer->bind(currentCommandBuffer);

        std::array<VkDescriptorSet, 2> descriptorSets = {
            perFrameDescriptorSets[currentFrame],
            mesh.descriptorSet
        };

        constexpr uint32_t firstSet = 0;
        constexpr uint32_t dynamicOffsetCount = 0;
        constexpr uint32_t* dynamicOffsets = nullptr;
        vkCmdBindDescriptorSets(
            currentCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphicsPipeline->getLayout(),
            firstSet,
            descriptorSets.size(),
            descriptorSets.data(),
            dynamicOffsetCount,
            dynamicOffsets
        );
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

    void Renderer::createDescriptorSetLayouts() {
        VkDescriptorSetLayoutBinding uniformBufferLayoutBinding{};
        uniformBufferLayoutBinding.binding = 0;
        uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferLayoutBinding.descriptorCount = 1;
        uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo perFrameLayoutCreateInfo{};
        perFrameLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        perFrameLayoutCreateInfo.bindingCount = 1;
        perFrameLayoutCreateInfo.pBindings = &uniformBufferLayoutBinding;

        BL_ASSERT_THROW_VK_SUCCESS(device->createDescriptorSetLayout(&perFrameLayoutCreateInfo, &perFrameDescriptorSetLayout));

        VkDescriptorSetLayoutBinding textureSamplerLayoutBinding{};
        textureSamplerLayoutBinding.binding = 0;
        textureSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureSamplerLayoutBinding.descriptorCount = MAX_DESCRIPTORS_PER_STAGE; // Must match uniform sampler2D textureSamplers[...] @ fragment shader
        textureSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo perMeshLayoutCreateInfo{};
        perMeshLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        perMeshLayoutCreateInfo.bindingCount = 1;
        perMeshLayoutCreateInfo.pBindings = &textureSamplerLayoutBinding;

        BL_ASSERT_THROW_VK_SUCCESS(device->createDescriptorSetLayout(&perMeshLayoutCreateInfo, &perMeshDescriptorSetLayout));
    }

    void Renderer::createDescriptorPools() {
        VkDescriptorPoolSize perFrameDescriptorPoolSize{};
        perFrameDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        perFrameDescriptorPoolSize.descriptorCount = (uint32_t) MAX_FRAMES_IN_FLIGHT;

        VkDescriptorPoolCreateInfo perFrameDescriptorPoolCreateInfo{};
        perFrameDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        perFrameDescriptorPoolCreateInfo.poolSizeCount = 1;
        perFrameDescriptorPoolCreateInfo.pPoolSizes = &perFrameDescriptorPoolSize;
        perFrameDescriptorPoolCreateInfo.maxSets = (uint32_t) MAX_FRAMES_IN_FLIGHT;

        BL_ASSERT_THROW_VK_SUCCESS(device->createDescriptorPool(&perFrameDescriptorPoolCreateInfo, &perFrameDescriptorPool));

        VkDescriptorPoolSize perMeshDescriptorPoolSize{};
        perMeshDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        perMeshDescriptorPoolSize.descriptorCount = (uint32_t) MAX_MESHES;

        VkDescriptorPoolCreateInfo perMeshDescriptorPoolCreateInfo{};
        perMeshDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        perMeshDescriptorPoolCreateInfo.poolSizeCount = 1;
        perMeshDescriptorPoolCreateInfo.pPoolSizes = &perMeshDescriptorPoolSize;
        perMeshDescriptorPoolCreateInfo.maxSets = (uint32_t) MAX_MESHES;

        BL_ASSERT_THROW_VK_SUCCESS(device->createDescriptorPool(&perMeshDescriptorPoolCreateInfo, &perMeshDescriptorPool));
    }

    void Renderer::createDescriptorSets() {
        perFrameDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        std::vector<VkDescriptorSetLayout> perFrameDescriptorSetLayouts(perFrameDescriptorSets.size(), perFrameDescriptorSetLayout);

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = perFrameDescriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = perFrameDescriptorSets.size();
        descriptorSetAllocateInfo.pSetLayouts = perFrameDescriptorSetLayouts.data();

        BL_ASSERT_THROW_VK_SUCCESS(device->allocateDescriptorSets(&descriptorSetAllocateInfo, perFrameDescriptorSets.data()));

        for (uint32_t i = 0; i < perFrameDescriptorSets.size(); i++) {
            VkDescriptorBufferInfo descriptorBufferInfo{};
            descriptorBufferInfo.buffer = *uniformBuffers[i];
            descriptorBufferInfo.offset = 0;
            descriptorBufferInfo.range = sizeof(UniformBufferData);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = perFrameDescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &descriptorBufferInfo;

            device->updateDescriptorSets(1, &descriptorWrite);
        }
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

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {
            perFrameDescriptorSetLayout,
            perMeshDescriptorSetLayout
        };

        VulkanGraphicsPipelineConfig graphicsPipelineConfig{};
        graphicsPipelineConfig.device = device;
        graphicsPipelineConfig.swapChain = swapChain;
        graphicsPipelineConfig.vertexShader = vertexShader;
        graphicsPipelineConfig.fragmentShader = fragmentShader;
        graphicsPipelineConfig.descriptorSetLayouts = descriptorSetLayouts;
        graphicsPipeline = new VulkanGraphicsPipeline(graphicsPipelineConfig);
    }

    void Renderer::destroyGraphicsPipelineObjects() const {
        delete graphicsPipeline;
        delete fragmentShader;
        delete vertexShader;
    }

}

