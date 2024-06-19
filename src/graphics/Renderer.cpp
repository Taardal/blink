#include "pch.h"
#include "Renderer.h"
#include "VulkanImage.h"
#include "window/KeyEvent.h"

namespace Blink {
    Renderer::Renderer(const RendererConfig& config) : config(config) {
        createCommandObjects();
        createSwapChain();
        createUniformBuffers();
        createDescriptorObjects();
        createGraphicsPipelines();
    }

    Renderer::~Renderer() {
        destroyGraphicsPipelines();
        destroyDescriptorObjects();
        destroyUniformBuffers();
        destroySwapChain();
        destroyCommandObjects();
    }

    void Renderer::waitUntilIdle() const {
        BL_ASSERT_THROW_VK_SUCCESS(config.device->waitUntilIdle());
    }

    void Renderer::onEvent(Event& event) {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::F12) {
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
        return true;
    }

    void Renderer::setViewProjection(const ViewProjection& viewProjection) const {
        VulkanUniformBuffer* uniformBuffer = viewProjectionUniformBuffers[currentFrame];

        ViewProjectionUniformBufferData uniformBufferData{};
        uniformBufferData.view = std::move(viewProjection.view);
        uniformBufferData.projection = std::move(viewProjection.projection);

        // Flip the Y-axis to align with Vulkan's coordinate system to avoid the image being rendered upside down.
        //
        // GLM was originally designed for OpenGL which uses a different coordinate system than Vulkan in clip space.
        //
        // OpenGL (right-handed)
        // - Right:   Positive X
        // - Up:      Positive Y <-- Opposite from Vulkan
        // - Forward: Positive Z
        //
        // Vulkan (right-handed)
        // - Right:   Positive X
        // - Up:      Negative Y <-- Opposite from OpenGL
        // - Forward: Positive Z
        //
        // The easiest way to compensate for the difference in up-direction is to flip the sign on the scaling factor
        // of the Y rotationAxis in the projection matrix.
        //
        // This change causes the vertices to be drawn in counter-clockwise order instead of clockwise order.
        // This causes backface culling to kick in and prevents any geometry from being drawn.
        //
        // To fix this the graphics pipeline's rasterization state should have a counter clockwise front-facing triangle
        // orientation to be used for culling:
        //
        // VkPipelineRasterizationStateCreateInfo rasterizationState{};
        // rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
        // rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        //
        uniformBufferData.projection[1][1] *= -1;

        uniformBuffer->setData(&uniformBufferData);
    }

    void Renderer::renderSkybox(const std::shared_ptr<Skybox>& skybox) const {
        skyboxGraphicsPipeline->bind(currentCommandBuffer);
        skybox->vertexBuffer->bind(currentCommandBuffer);
        skybox->indexBuffer->bind(currentCommandBuffer);

        std::array<VkDescriptorSet, 2> descriptorSets = {
            this->viewProjectionDescriptorSets[currentFrame], // Per frame descriptor set
            skybox->descriptorSet // Per mesh descriptor set
        };

        constexpr uint32_t firstSet = 0;
        constexpr uint32_t dynamicOffsetCount = 0;
        constexpr uint32_t* dynamicOffsets = nullptr;
        vkCmdBindDescriptorSets(
            currentCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            skyboxGraphicsPipeline->getLayout(),
            firstSet,
            descriptorSets.size(),
            descriptorSets.data(),
            dynamicOffsetCount,
            dynamicOffsets
        );

        constexpr uint32_t instanceCount = 1;
        constexpr uint32_t firstIndex = 0;
        constexpr uint32_t vertexOffset = 0;
        constexpr uint32_t firstInstance = 0;
        vkCmdDrawIndexed(
            currentCommandBuffer,
            (uint32_t) skybox->indices.size(),
            instanceCount,
            firstIndex,
            vertexOffset,
            firstInstance
        );
    }

    void Renderer::renderMesh(const std::shared_ptr<Mesh>& mesh) const {
        meshGraphicsPipeline->bind(currentCommandBuffer);
        mesh->vertexBuffer->bind(currentCommandBuffer);
        mesh->indexBuffer->bind(currentCommandBuffer);

        constexpr uint32_t offset = 0;
        vkCmdPushConstants(
            currentCommandBuffer,
            meshGraphicsPipeline->getLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            offset,
            sizeof(MeshPushConstantData),
            &mesh->model
        );

        std::array<VkDescriptorSet, 2> descriptorSets = {
            this->viewProjectionDescriptorSets[currentFrame], // Per frame descriptor set
            mesh->descriptorSet // Per mesh descriptor set
        };

        constexpr uint32_t firstSet = 0;
        constexpr uint32_t dynamicOffsetCount = 0;
        constexpr uint32_t* dynamicOffsets = nullptr;
        vkCmdBindDescriptorSets(
            currentCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            meshGraphicsPipeline->getLayout(),
            firstSet,
            descriptorSets.size(),
            descriptorSets.data(),
            dynamicOffsetCount,
            dynamicOffsets
        );

        constexpr uint32_t instanceCount = 1;
        constexpr uint32_t firstIndex = 0;
        constexpr uint32_t vertexOffset = 0;
        constexpr uint32_t firstInstance = 0;
        vkCmdDrawIndexed(
            currentCommandBuffer,
            (uint32_t) mesh->indices.size(),
            instanceCount,
            firstIndex,
            vertexOffset,
            firstInstance
        );
    }

    void Renderer::endFrame() {
        swapChain->endRenderPass(currentCommandBuffer);
        BL_ASSERT_THROW_VK_SUCCESS(currentCommandBuffer.end());
        swapChain->endFrame(currentCommandBuffer);
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::reloadShaders() {
        BL_ASSERT_THROW_VK_SUCCESS(config.device->waitUntilIdle());
        config.shaderManager->reloadShaders();
        destroyGraphicsPipelines();
        createGraphicsPipelines();
        BL_LOG_INFO("Reloaded shaders");
    }

    void Renderer::createCommandObjects() {
        VulkanCommandPoolConfig commandPoolConfig{};
        commandPoolConfig.device = config.device;
        commandPool = new VulkanCommandPool(commandPoolConfig);

        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        BL_ASSERT_THROW_VK_SUCCESS(commandPool->allocateCommandBuffers(&commandBuffers));
    }

    void Renderer::destroyCommandObjects() const {
        delete commandPool;
    }

    void Renderer::createSwapChain() {
        VulkanSwapChainConfig swapChainConfig{};
        swapChainConfig.window = config.window;
        swapChainConfig.vulkanApp = config.vulkanApp;
        swapChainConfig.device = config.device;
        swapChainConfig.commandPool = commandPool;
        swapChain = new VulkanSwapChain(swapChainConfig);
    }

    void Renderer::destroySwapChain() const {
        delete swapChain;
    }

    void Renderer::createUniformBuffers() {
        viewProjectionUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for (uint32_t i = 0; i < viewProjectionUniformBuffers.size(); i++) {
            VulkanUniformBufferConfig uniformBufferConfig{};
            uniformBufferConfig.device = config.device;
            uniformBufferConfig.commandPool = commandPool;
            uniformBufferConfig.size = sizeof(ViewProjectionUniformBufferData);
            viewProjectionUniformBuffers[i] = new VulkanUniformBuffer(uniformBufferConfig);
        }
    }

    void Renderer::destroyUniformBuffers() {
        for (VulkanUniformBuffer* uniformBuffer : viewProjectionUniformBuffers) {
            delete uniformBuffer;
        }
        viewProjectionUniformBuffers.clear();
    }

    void Renderer::createDescriptorObjects() {
        // Descriptor pool
        VkDescriptorPoolSize descriptorPoolSize{};
        descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSize.descriptorCount = (uint32_t) MAX_FRAMES_IN_FLIGHT;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = 1;
        descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;
        descriptorPoolCreateInfo.maxSets = (uint32_t) MAX_FRAMES_IN_FLIGHT;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createDescriptorPool(&descriptorPoolCreateInfo, &viewProjectionDescriptorPool));

        // Descriptor set layout
        VkDescriptorSetLayoutBinding uniformBufferLayoutBinding{};
        uniformBufferLayoutBinding.binding = 0;
        uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferLayoutBinding.descriptorCount = 1;
        uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = 1;
        descriptorSetLayoutCreateInfo.pBindings = &uniformBufferLayoutBinding;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, &viewProjectionDescriptorSetLayout));

        // Descriptor set
        viewProjectionDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(viewProjectionDescriptorSets.size(), viewProjectionDescriptorSetLayout);

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = viewProjectionDescriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = viewProjectionDescriptorSets.size();
        descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

        BL_ASSERT_THROW_VK_SUCCESS(config.device->allocateDescriptorSets(&descriptorSetAllocateInfo, viewProjectionDescriptorSets.data()));

        for (uint32_t i = 0; i < viewProjectionDescriptorSets.size(); i++) {
            VkDescriptorBufferInfo descriptorBufferInfo{};
            descriptorBufferInfo.buffer = *viewProjectionUniformBuffers[i];
            descriptorBufferInfo.offset = 0;
            descriptorBufferInfo.range = sizeof(ViewProjectionUniformBufferData);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = viewProjectionDescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &descriptorBufferInfo;

            config.device->updateDescriptorSets(1, &descriptorWrite);
        }
    }

    void Renderer::destroyDescriptorObjects() const {
        config.device->destroyDescriptorPool(viewProjectionDescriptorPool);
        config.device->destroyDescriptorSetLayout(viewProjectionDescriptorSetLayout);
    }

    void Renderer::createGraphicsPipelines() {
        // Mesh
        {
            std::shared_ptr<VulkanShader> vertexShader = config.shaderManager->getShader("shaders/mesh.vert.spv");
            std::shared_ptr<VulkanShader> fragmentShader = config.shaderManager->getShader("shaders/mesh.frag.spv");

            VkVertexInputBindingDescription vertexBindingDescription = MeshVertex::getBindingDescription();
            std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions = MeshVertex::getAttributeDescriptions();

            std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {
                viewProjectionDescriptorSetLayout, // Per frame descriptor set layout
                config.meshManager->getDescriptorSetLayout() // Per mesh descriptor set layout
            };

            VkPushConstantRange pushConstantRange{};
            pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            pushConstantRange.offset = 0;
            pushConstantRange.size = sizeof(MeshPushConstantData);

            std::vector<VkPushConstantRange> pushConstantRanges = {
                pushConstantRange,
            };

            VulkanGraphicsPipelineConfig graphicsPipelineConfig{};
            graphicsPipelineConfig.device = config.device;
            graphicsPipelineConfig.renderPass = swapChain->getRenderPass();
            graphicsPipelineConfig.vertexShader = vertexShader;
            graphicsPipelineConfig.fragmentShader = fragmentShader;
            graphicsPipelineConfig.vertexBindingDescription = &vertexBindingDescription;
            graphicsPipelineConfig.vertexAttributeDescriptions = &vertexAttributeDescriptions;
            graphicsPipelineConfig.descriptorSetLayouts = &descriptorSetLayouts;
            graphicsPipelineConfig.pushConstantRanges = &pushConstantRanges;
            graphicsPipelineConfig.depthTestEnabled = true;

            meshGraphicsPipeline = new VulkanGraphicsPipeline(graphicsPipelineConfig);
        }
        // Skybox
        {
            std::shared_ptr<VulkanShader> vertexShader = config.shaderManager->getShader("shaders/skybox.vert.spv");
            std::shared_ptr<VulkanShader> fragmentShader = config.shaderManager->getShader("shaders/skybox.frag.spv");

            VkVertexInputBindingDescription vertexBindingDescription = SkyboxVertex::getBindingDescription();
            std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions = SkyboxVertex::getAttributeDescriptions();

            std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {
                viewProjectionDescriptorSetLayout, // Per frame descriptor set layout
                config.skyboxManager->getDescriptorSetLayout(),
            };

            VulkanGraphicsPipelineConfig graphicsPipelineConfig{};
            graphicsPipelineConfig.device = config.device;
            graphicsPipelineConfig.renderPass = swapChain->getRenderPass();
            graphicsPipelineConfig.vertexShader = vertexShader;
            graphicsPipelineConfig.fragmentShader = fragmentShader;
            graphicsPipelineConfig.vertexBindingDescription = &vertexBindingDescription;
            graphicsPipelineConfig.vertexAttributeDescriptions = &vertexAttributeDescriptions;
            graphicsPipelineConfig.descriptorSetLayouts = &descriptorSetLayouts;
            graphicsPipelineConfig.depthTestEnabled = false;

            skyboxGraphicsPipeline = new VulkanGraphicsPipeline(graphicsPipelineConfig);
        }
    }

    void Renderer::destroyGraphicsPipelines() const {
        delete skyboxGraphicsPipeline;
        delete meshGraphicsPipeline;
    }
}

