#include "pch.h"
#include "Renderer.h"
#include "UniformBufferObject.h"
#include "window/KeyEvent.h"

#include <chrono>

#include "VulkanImage.h"

namespace Blink {

    Renderer:: Renderer(
        const AppConfig& appConfig,
        FileSystem* fileSystem,
        Window* window
    ) : fileSystem(fileSystem),
        window(window)
    {
        //
        // VULKAN INSTANCE
        //

        VulkanAppConfig vulkanAppConfig{};
        vulkanAppConfig.window = window;
        vulkanAppConfig.applicationName = appConfig.name;
        vulkanAppConfig.engineName = appConfig.name;
        vulkanAppConfig.validationLayersEnabled = true;
        BL_TRY(vulkanApp = new VulkanApp(vulkanAppConfig));
        BL_LOG_INFO("Created Vulkan app");

        //
        // PHYSICAL DEVICE
        //

        VulkanPhysicalDeviceConfig physicalDeviceConfig{};
        physicalDeviceConfig.vulkanApp = vulkanApp;
        BL_TRY(physicalDevice = new VulkanPhysicalDevice(physicalDeviceConfig));
        BL_LOG_INFO("Created Vulkan physical device");

        //
        // LOGICAL DEVICE
        //

        VulkanDeviceConfig deviceConfig{};
        deviceConfig.physicalDevice = physicalDevice;
        BL_TRY(device = new VulkanDevice(deviceConfig));
        BL_LOG_INFO("Created Vulkan device app");

        //
        // COMMAND POOL
        //

        VulkanCommandPoolConfig commandPoolConfig{};
        commandPoolConfig.physicalDevice = physicalDevice;
        commandPoolConfig.device = device;
        BL_TRY(commandPool = new VulkanCommandPool(commandPoolConfig));
        BL_LOG_INFO("Created Vulkan command pool");

        //
        // COMMAND BUFFERS
        //

        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        BL_ASSERT_THROW_VK_SUCCESS(commandPool->allocateCommandBuffers(&commandBuffers));
        BL_LOG_INFO("Created [{}] Vulkan command buffer(s)", commandBuffers.size());

        //
        // SWAP CHAIN
        //

        VulkanSwapChainConfig swapChainConfig{};
        swapChainConfig.window = window;
        swapChainConfig.vulkanApp = vulkanApp;
        swapChainConfig.physicalDevice = physicalDevice;
        swapChainConfig.device = device;
        swapChainConfig.commandPool = commandPool;
        BL_TRY(swapChain = new VulkanSwapChain(swapChainConfig));
        BL_LOG_INFO("Created Vulkan swap chain");

        //
        // VERTEX SHADER
        //

        VulkanShaderConfig vertexShaderConfig{};
        vertexShaderConfig.device = device;
        BL_TRY(vertexShader = new VulkanShader(vertexShaderConfig));

        //
        // FRAGMENT SHADER
        //

        VulkanShaderConfig fragmentShaderConfig{};
        fragmentShaderConfig.device = device;
        BL_TRY(fragmentShader = new VulkanShader(fragmentShaderConfig));

        //
        // GRAPHICS PIPELINE
        //

        VulkanGraphicsPipelineConfig graphicsPipelineConfig{};
        graphicsPipelineConfig.device = device;
        graphicsPipelineConfig.swapChain = swapChain;
        graphicsPipelineConfig.vertexShader = vertexShader;
        graphicsPipelineConfig.fragmentShader = fragmentShader;
        BL_TRY(graphicsPipeline = new VulkanGraphicsPipeline(graphicsPipelineConfig));

        //
        // VERTEX BUFFER
        //

        VulkanVertexBufferConfig vertexBufferConfig{};
        vertexBufferConfig.physicalDevice = physicalDevice;
        vertexBufferConfig.device = device;
        vertexBufferConfig.commandPool = commandPool;
        vertexBuffer = new VulkanVertexBuffer(vertexBufferConfig);
        BL_ASSERT_THROW(vertexBuffer->initialize(vertices));

        //
        // INDEX BUFFER
        //

        VulkanIndexBufferConfig indexBufferConfig{};
        indexBufferConfig.physicalDevice = physicalDevice;
        indexBufferConfig.device = device;
        indexBufferConfig.commandPool = commandPool;
        indexBuffer = new VulkanIndexBuffer(indexBufferConfig);
        BL_ASSERT_THROW(indexBuffer->initialize(indices));

        //
        // UNIFORM BUFFERS
        //

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VulkanUniformBufferConfig uniformBufferConfig{};
            uniformBufferConfig.physicalDevice = physicalDevice;
            uniformBufferConfig.device = device;
            uniformBufferConfig.commandPool = commandPool;
            auto uniformBuffer = new VulkanUniformBuffer(uniformBufferConfig);
            BL_ASSERT_THROW(uniformBuffer->initialize(sizeof(UniformBufferObject)));
            uniformBuffers.push_back(uniformBuffer);
        }

        //
        // TEXTURE IMAGE
        //

        Image image = fileSystem->readImage("textures/sculpture.png");

        VulkanImageConfig textureImageConfig{};
        textureImageConfig.physicalDevice = physicalDevice;
        textureImageConfig.device = device;
        textureImageConfig.commandPool = commandPool;
        textureImageConfig.width = image.width;
        textureImageConfig.height = image.height;
        textureImageConfig.format = VK_FORMAT_R8G8B8A8_SRGB;
        textureImageConfig.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureImageConfig.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
        textureImageConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        BL_TRY(textureImage = new VulkanImage(textureImageConfig));
        BL_TRY(textureImage->setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL));
        BL_TRY(textureImage->setData(image));
        BL_TRY(textureImage->setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

        image.free();

        //
        // TEXTURE SAMPLER
        //

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

        //
        // DESCRIPTOR LAYOUTS
        //

        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        BL_ASSERT_THROW_VK_SUCCESS(device->createDescriptorSetLayout(&layoutInfo, &descriptorSetLayout));

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

        //
        // DESCRIPTOR POOL
        //

        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        BL_ASSERT_THROW_VK_SUCCESS(device->createDescriptorPool(&poolInfo, &descriptorPool));

        //
        // DESCRIPTOR SETS
        //

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

        descriptorSets.resize(descriptorSetAllocateInfo.descriptorSetCount);
        BL_ASSERT_THROW_VK_SUCCESS(device->allocateDescriptorSets(&descriptorSetAllocateInfo, descriptorSets.data()));

        for (size_t i = 0; i < descriptorSets.size(); i++) {
            VulkanUniformBuffer* uniformBuffer = uniformBuffers[i];

            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = *uniformBuffer;
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureImage->getImageView();
            imageInfo.sampler = textureSampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            device->updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data());
        }

        //
        // GRAPHICS PIPELINE OBJCETS (PIPELINE + SHADERS)
        //

        BL_TRY(initializeGraphicsPipelineObjects());
    }

    Renderer::~Renderer() {
        //
        // TEXTURE RESOURCES
        //

        device->destroySampler(textureSampler);
        delete textureImage;

        //
        // VERTEX & INDEX BUFFERS
        //

        indexBuffer->terminate();
        delete indexBuffer;
        vertexBuffer->terminate();
        delete vertexBuffer;

        //
        // GRAPHICS PIPELINE & SHADERS
        //

        terminateGraphicsPipelineObjects();
        delete graphicsPipeline;
        delete fragmentShader;
        delete vertexShader;

        //
        // DESCRIPTOR RESOURCES
        //

        device->destroyDescriptorPool(descriptorPool);
        device->destroyDescriptorSetLayout(descriptorSetLayout);

        //
        // UNIFORM BUFFERS
        //

        for (VulkanUniformBuffer* uniformBuffer : uniformBuffers) {
            uniformBuffer->terminate();
            delete uniformBuffer;
        }

        delete swapChain;
        delete commandPool;
        delete device;
        delete physicalDevice;
        delete vulkanApp;
    }

    void Renderer::render(const Frame& frame) {
        if (!swapChain->beginFrame(currentFrame)) {
            return;
        }

        const VulkanCommandBuffer& commandBuffer = commandBuffers[currentFrame];
        VulkanUniformBuffer* uniformBuffer = uniformBuffers[currentFrame];
        VkDescriptorSet descriptorSet = descriptorSets[currentFrame];

        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.begin());

        swapChain->beginRenderPass(commandBuffer);
        graphicsPipeline->bind(commandBuffer);
        vertexBuffer->bind(commandBuffer);
        indexBuffer->bind(commandBuffer);
        bindDescriptorSet(descriptorSet, commandBuffer);
        drawIndexed(commandBuffer);
        swapChain->endRenderPass(commandBuffer);

        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.end());

        setUniformData(uniformBuffer, frame);

        swapChain->endFrame(commandBuffer);
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::waitUntilIdle() const {
        BL_ASSERT_THROW_VK_SUCCESS(device->waitUntilIdle());
    }

    void Renderer::onEvent(Event& event) {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::O) {
            recompileShaders();
            return;
        }
        swapChain->onEvent(event);
    }

    void Renderer::setUniformData(VulkanUniformBuffer* uniformBuffer, const Frame& frame) const {
        UniformBufferObject ubo{};
        ubo.model = frame.model;
        ubo.view = frame.view;
        ubo.proj = frame.projection;

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
        ubo.proj[1][1] *= -1;

        uniformBuffer->setData(&ubo);
    }

    void Renderer::bindDescriptorSet(VkDescriptorSet descriptorSet, const VulkanCommandBuffer& commandBuffer) const {
        VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        VkPipelineLayout pipelineLayout = graphicsPipeline->getLayout();
        constexpr uint32_t firstSet = 0;
        constexpr uint32_t descriptorSetCount = 1;
        constexpr uint32_t dynamicOffsetCount = 0;
        constexpr uint32_t* dynamicOffsets = nullptr;
        vkCmdBindDescriptorSets(
            commandBuffer,
            pipelineBindPoint,
            pipelineLayout,
            firstSet,
            descriptorSetCount,
            &descriptorSet,
            dynamicOffsetCount,
            dynamicOffsets
        );
    }

    void Renderer::drawIndexed(const VulkanCommandBuffer& commandBuffer) const {
        constexpr uint32_t instanceCount = 1;
        constexpr uint32_t firstIndex = 0;
        constexpr uint32_t vertexOffset = 0;
        constexpr uint32_t firstInstance = 0;
        vkCmdDrawIndexed(
            commandBuffer,
            (uint32_t) indices.size(),
            instanceCount,
            firstIndex,
            vertexOffset,
            firstInstance
        );
    }

    void Renderer::recompileShaders() {
        compileShaders();
        terminateGraphicsPipelineObjects();
        BL_TRY(initializeGraphicsPipelineObjects());
    }

    void Renderer::compileShaders() {
        std::stringstream ss;
        ss << "cmake";
        ss << " -D SHADERS_SOURCE_DIR=" << CMAKE_SHADERS_SOURCE_DIR;
        ss << " -D SHADERS_OUTPUT_DIR=" << CMAKE_SHADERS_OUTPUT_DIR;
        ss << " -P " << CMAKE_SCRIPTS_DIR << "/compile_shaders.cmake";
        std::string command = ss.str();
        std::system(command.c_str());
    }

    void Renderer::initializeGraphicsPipelineObjects() const {
        BL_ASSERT_THROW(vertexShader->initialize(fileSystem->readBytes("shaders/shader.vert.spv")));
        BL_ASSERT_THROW(fragmentShader->initialize(fileSystem->readBytes("shaders/shader.frag.spv")));
        BL_ASSERT_THROW(graphicsPipeline->initialize(descriptorSetLayout));
    }

    void Renderer::terminateGraphicsPipelineObjects() const {
        graphicsPipeline->terminate();
        fragmentShader->terminate();
        vertexShader->terminate();
    }

}

