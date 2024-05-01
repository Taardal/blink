#include "pch.h"
#include "Renderer.h"
#include "PushConstantData.h"
#include "UniformBufferData.h"
#include "VulkanImage.h"
#include "window/KeyEvent.h"

namespace Blink {
    Renderer::Renderer(const RendererConfig& config) : config(config) {
        VulkanSwapChainConfig swapChainConfig{};
        swapChainConfig.window = config.window;
        swapChainConfig.vulkanApp = config.vulkanApp;
        swapChainConfig.device = config.device;
        swapChainConfig.commandPool = config.commandPool;
        swapChain = new VulkanSwapChain(swapChainConfig);

        createCommandObjects();
        createUniformBuffers();
        createDescriptorObjects();
        createGraphicsPipelineObjects();
    }

    Renderer::~Renderer() {
        destroyGraphicsPipelineObjects();
        destroyDescriptorObjects();
        destroyUniformBuffers();
        destroyCommandObjects();
        delete swapChain;
    }

    void Renderer::waitUntilIdle() const {
        BL_ASSERT_THROW_VK_SUCCESS(config.device->waitUntilIdle());
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

    void Renderer::renderMesh(std::shared_ptr<Mesh> mesh, const ViewProjection& viewProjection) const {
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

    void Renderer::setPushConstantData(std::shared_ptr<Mesh> mesh) const {
        PushConstantData pushConstantData{};
        pushConstantData.model = mesh->model;
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

    void Renderer::bindMesh(std::shared_ptr<Mesh> mesh) const {
        mesh->vertexBuffer->bind(currentCommandBuffer);
        mesh->indexBuffer->bind(currentCommandBuffer);

        std::array<VkDescriptorSet, 2> descriptorSets = {
            this->descriptorSets[currentFrame], // Per frame descriptor set
            mesh->descriptorSet // Per mesh descriptor set
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

    void Renderer::drawMeshIndexed(std::shared_ptr<Mesh> mesh) const {
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

    void Renderer::reloadShaders() {
        BL_ASSERT_THROW_VK_SUCCESS(config.device->waitUntilIdle());
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

    void Renderer::createCommandObjects() {
        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        BL_ASSERT_THROW_VK_SUCCESS(config.commandPool->allocateCommandBuffers(&commandBuffers));
    }

    void Renderer::destroyCommandObjects() const {
    }

    void Renderer::createUniformBuffers() {
        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for (uint32_t i = 0; i < uniformBuffers.size(); i++) {
            VulkanUniformBufferConfig uniformBufferConfig{};
            uniformBufferConfig.device = config.device;
            uniformBufferConfig.commandPool = config.commandPool;
            uniformBufferConfig.size = sizeof(UniformBufferData);
            uniformBuffers[i] = new VulkanUniformBuffer(uniformBufferConfig);
        }
    }

    void Renderer::destroyUniformBuffers() {
        for (VulkanUniformBuffer* uniformBuffer : uniformBuffers) {
            delete uniformBuffer;
        }
        uniformBuffers.clear();
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

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createDescriptorPool(&descriptorPoolCreateInfo, &descriptorPool));

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

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, &descriptorSetLayout));

        // Descriptor set
        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(descriptorSets.size(), descriptorSetLayout);

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = descriptorSets.size();
        descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

        BL_ASSERT_THROW_VK_SUCCESS(config.device->allocateDescriptorSets(&descriptorSetAllocateInfo, descriptorSets.data()));

        for (uint32_t i = 0; i < descriptorSets.size(); i++) {
            VkDescriptorBufferInfo descriptorBufferInfo{};
            descriptorBufferInfo.buffer = *uniformBuffers[i];
            descriptorBufferInfo.offset = 0;
            descriptorBufferInfo.range = sizeof(UniformBufferData);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &descriptorBufferInfo;

            config.device->updateDescriptorSets(1, &descriptorWrite);
        }
    }

    void Renderer::destroyDescriptorObjects() const {
        config.device->destroyDescriptorPool(descriptorPool);
        config.device->destroyDescriptorSetLayout(descriptorSetLayout);
    }

    void Renderer::createGraphicsPipelineObjects() {
        VulkanShaderConfig vertexShaderConfig{};
        vertexShaderConfig.device = config.device;
        vertexShaderConfig.bytes = config.fileSystem->readBytes("shaders/shader.vert.spv");
        vertexShader = new VulkanShader(vertexShaderConfig);

        VulkanShaderConfig fragmentShaderConfig{};
        fragmentShaderConfig.device = config.device;
        fragmentShaderConfig.bytes = config.fileSystem->readBytes("shaders/shader.frag.spv");
        fragmentShader = new VulkanShader(fragmentShaderConfig);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {
            descriptorSetLayout, // Per frame descriptor set layout
            config.resourceLoader->getDescriptorSetLayout() // Per mesh descriptor set layout
        };

        VulkanGraphicsPipelineConfig graphicsPipelineConfig{};
        graphicsPipelineConfig.device = config.device;
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

