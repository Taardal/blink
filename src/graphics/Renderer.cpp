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

        //
        // PHYSICAL DEVICE
        //

        VulkanPhysicalDeviceConfig physicalDeviceConfig{};
        physicalDeviceConfig.vulkanApp = vulkanApp;
        BL_TRY(physicalDevice = new VulkanPhysicalDevice(physicalDeviceConfig));
        VkFormat depthFormat = physicalDevice->getDepthFormat();

        //
        // LOGICAL DEVICE
        //

        VulkanDeviceConfig deviceConfig{};
        deviceConfig.physicalDevice = physicalDevice;
        BL_TRY(device = new VulkanDevice(deviceConfig));

        //
        // SWAP CHAIN
        //

        VulkanSwapChainConfig swapChainConfig{};
        swapChainConfig.window = window;
        swapChainConfig.vulkanApp = vulkanApp;
        swapChainConfig.physicalDevice = physicalDevice;
        swapChainConfig.device = device;
        BL_TRY(swapChain = new VulkanSwapChain(swapChainConfig));

        VkExtent2D swapChainExtent = swapChain->getExtent();

        //
        // RENDER PASS
        //

        VulkanRenderPassConfig renderPassConfig{};
        renderPassConfig.physicalDevice = physicalDevice;
        renderPassConfig.device = device;
        renderPassConfig.swapChain = swapChain;
        BL_TRY(renderPass = new VulkanRenderPass(renderPassConfig));

        //
        // COMMAND POOL
        //

        VulkanCommandPoolConfig commandPoolConfig{};
        commandPoolConfig.physicalDevice = physicalDevice;
        commandPoolConfig.device = device;
        BL_TRY(commandPool = new VulkanCommandPool(commandPoolConfig));

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
        graphicsPipelineConfig.renderPass = renderPass;
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
        // DEPTH IMAGE
        //

        VkImageCreateInfo depthImageCreateInfo{};
        depthImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        depthImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        depthImageCreateInfo.extent.width = swapChainExtent.width;
        depthImageCreateInfo.extent.height = swapChainExtent.height;
        depthImageCreateInfo.extent.depth = 1;
        depthImageCreateInfo.mipLevels = 1;
        depthImageCreateInfo.arrayLayers = 1;
        depthImageCreateInfo.format = depthFormat;
        depthImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        depthImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        depthImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VulkanImageConfig depthImageConfig{};
        depthImageConfig.createInfo = &depthImageCreateInfo;
        depthImageConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        depthImageConfig.physicalDevice = physicalDevice;
        depthImageConfig.device = device;
        depthImageConfig.commandPool = commandPool;

        BL_TRY(depthImage = new VulkanImage(depthImageConfig));
        BL_TRY(depthImage->setLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));

        //
        // DEPTH IMAGE VIEW
        //

        VkImageViewCreateInfo depthImageViewCreateInfo{};
        depthImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        depthImageViewCreateInfo.image = *depthImage;
        depthImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        depthImageViewCreateInfo.format = depthFormat;
        depthImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        depthImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        depthImageViewCreateInfo.subresourceRange.levelCount = 1;
        depthImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        depthImageViewCreateInfo.subresourceRange.layerCount = 1;

        BL_ASSERT_THROW_VK_SUCCESS(device->createImageView(&depthImageViewCreateInfo, &depthImageView));

        //
        // TEXTURE IMAGE
        //

        Image image = fileSystem->readImage("textures/sculpture.png");

        VkImageCreateInfo textureImageCreateInfo{};
        textureImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        textureImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        textureImageCreateInfo.extent.width = image.width;
        textureImageCreateInfo.extent.height = image.height;
        textureImageCreateInfo.extent.depth = 1;
        textureImageCreateInfo.mipLevels = 1;
        textureImageCreateInfo.arrayLayers = 1;
        textureImageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        textureImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        textureImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        textureImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        textureImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VulkanImageConfig textureImageConfig{};
        textureImageConfig.createInfo = &textureImageCreateInfo;
        textureImageConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        textureImageConfig.physicalDevice = physicalDevice;
        textureImageConfig.device = device;
        textureImageConfig.commandPool = commandPool;

        BL_TRY(textureImage = new VulkanImage(textureImageConfig));
        BL_TRY(textureImage->setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL));
        BL_TRY(textureImage->setData(image));
        BL_TRY(textureImage->setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

        image.free();

        //
        // TEXTURE IMAGE VIEW
        //

        VkImageViewCreateInfo textureImageViewCreateInfo{};
        textureImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        textureImageViewCreateInfo.image = *textureImage;
        textureImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        textureImageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        textureImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        textureImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        textureImageViewCreateInfo.subresourceRange.levelCount = 1;
        textureImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        textureImageViewCreateInfo.subresourceRange.layerCount = 1;

        BL_ASSERT_THROW_VK_SUCCESS(device->createImageView(&textureImageViewCreateInfo, &textureImageView));

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
            imageInfo.imageView = textureImageView;
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
        // FIX ME
        // FIX ME
        // FIX ME

        // GRAPHICS PIPELINE OBJCETS (PIPELINE + SHADERS)
        //
        // FIX ME
        // FIX ME
        // FIX ME
        //

        if (!initializeGraphicsPipelineObjects()) {
            throw std::runtime_error(BL_TAG("Could not initialize graphics pipeline objects"));
        }

        //
        // FRAMEBUFFERS
        //

        const std::vector<VkImageView>& swapChainImageViews = swapChain->getImageViews();
        framebuffers.resize(swapChainImageViews.size());
        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            std::array<VkImageView, 2> attachments = {
                swapChainImageViews[i],
                depthImageView
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = *renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            BL_ASSERT_THROW_VK_SUCCESS(device->createFramebuffer(&framebufferInfo, &framebuffers[i]));
        }

        //
        // COMMAND BUFFERS
        //

        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        BL_ASSERT_THROW_VK_SUCCESS(commandPool->allocateCommandBuffers(&commandBuffers));

        //
        // SYNC OBJECTS
        //

        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            BL_ASSERT_THROW_VK_SUCCESS(device->createSemaphore(&semaphoreCreateInfo, &imageAvailableSemaphores[i]));
            BL_ASSERT_THROW_VK_SUCCESS(device->createSemaphore(&semaphoreCreateInfo, &renderFinishedSemaphores[i]));
            BL_ASSERT_THROW_VK_SUCCESS(device->createFence(&fenceCreateInfo, &inFlightFences[i]));
        }
    }

    Renderer::~Renderer() {
        device->waitUntilIdle();

        //
        // DEPTH RESOURCES
        //

        device->destroyImageView(depthImageView);
        delete depthImage;

        //
        // TEXTURE RESOURCES
        //

        device->destroySampler(textureSampler);
        device->destroyImageView(textureImageView);
        delete textureImage;

        //
        // SYNC RESOURCES
        //

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            device->destroySemaphore(imageAvailableSemaphores[i]);
            device->destroySemaphore(renderFinishedSemaphores[i]);
            device->destroyFence(inFlightFences[i]);
        }

        //
        // FRAMEBUFFERS
        //

        for (VkFramebuffer framebuffer : framebuffers) {
            device->destroyFramebuffer(framebuffer);
        }

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

        delete commandPool;
        delete renderPass;
        delete swapChain;
        delete device;
        delete physicalDevice;
        delete vulkanApp;
    }

    void Renderer::onEvent(Event& event) {
        if (event.type == EventType::WindowResize || event.type == EventType::WindowMinimize) {
            this->framebufferResized = true;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::O) {
            compileShaders();
            terminateGraphicsPipelineObjects();
            if (!initializeGraphicsPipelineObjects()) {
                throw std::runtime_error("Could not initialize graphics pipeline objects");
            }
        }
    }

    void Renderer::render(const Frame& frame) {
        //
        // Resources
        //

        VkFence inFlightFence = inFlightFences[currentFrame];
        VkSemaphore imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
        VkSemaphore renderFinishedSemaphore = renderFinishedSemaphores[currentFrame];
        VkCommandBuffer commandBuffer = commandBuffers[currentFrame];
        VulkanUniformBuffer* uniformBuffer = uniformBuffers[currentFrame];

        //
        // Acquisition
        //

        device->waitForFence(&inFlightFence);

        uint32_t imageIndex;
        VkResult nextImageResult = swapChain->acquireNextImage(imageAvailableSemaphore, &imageIndex);
        if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR) {
            BL_THROW("Could not acquire next image from swap chain");
        }

        device->resetFence(&inFlightFence);

        VkCommandBufferResetFlags commandBufferResetFlags = 0;
        vkResetCommandBuffer(commandBuffer, commandBufferResetFlags);

        //
        // Recording
        //

        recordCommandBuffer(commandBuffer, imageIndex);
        updateUniformBuffer(uniformBuffer, frame);

        //
        // Submission
        //

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {
                imageAvailableSemaphore
        };
        VkPipelineStageFlags waitStages[] = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VkSemaphore signalSemaphores[] = {
                renderFinishedSemaphore
        };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        uint32_t submitCount = 1;
        if (vkQueueSubmit(device->getGraphicsQueue(), submitCount, &submitInfo, inFlightFence) != VK_SUCCESS) {
            BL_THROW("Could not submit draw command buffer");
        }

        //
        // Presentation
        //

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {
                *swapChain
        };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        VkResult presentResult = vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        } else if (presentResult != VK_SUCCESS) {
            BL_THROW("Could not present image to swap chain");
        }

        //
        // Next frame
        //

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) const {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            BL_THROW("Could not begin recording command buffer");
        }

        renderPass->begin(commandBuffer, framebuffers.at(imageIndex));
        graphicsPipeline->bind(commandBuffer);
        vertexBuffer->bind(commandBuffer);
        indexBuffer->bind(commandBuffer);

        VkDescriptorSet descriptorSet = descriptorSets[currentFrame];
        VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        VkPipelineLayout pipelineLayout = graphicsPipeline->getLayout();
        constexpr uint32_t firstSet = 0;
        constexpr uint32_t descriptorSetCount = 1;
        constexpr uint32_t dynamicOffsetCount = 0;
        constexpr uint32_t* dynamicOffsets = nullptr;
        vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, pipelineLayout, firstSet, descriptorSetCount, &descriptorSet, dynamicOffsetCount, dynamicOffsets);

        const VkExtent2D& swapChainExtent = swapChain->getExtent();

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        constexpr uint32_t instanceCount = 1;
        constexpr uint32_t firstIndex = 0;
        constexpr uint32_t vertexOffset = 0;
        constexpr uint32_t firstInstance = 0;
        vkCmdDrawIndexed(commandBuffer, (uint32_t) indices.size(), instanceCount, firstIndex, vertexOffset, firstInstance);

        renderPass->end(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            BL_THROW("Could not end frame");
        }
    }

    void Renderer::updateUniformBuffer(VulkanUniformBuffer* uniformBuffer, const Frame& frame) {
        UniformBufferObject ubo{};
        ubo.model = frame.model;
        ubo.view = frame.view;
        ubo.proj = frame.projection;

        //
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
        //
        ubo.proj[1][1] *= -1;

        uniformBuffer->setData(&ubo);
    }

    bool Renderer::recreateSwapChain() {
        window->waitUntilNotMinimized();
        device->waitUntilIdle();

        terminateFramebuffers();
        delete depthImage;
        swapChain->terminate();

        physicalDevice->updateSwapChainInfo();

        if (!swapChain->initialize()) {
            BL_LOG_ERROR("Could not recreate swap chain");
            return false;
        }
        if (!initializeDepthResources()) {
            BL_LOG_ERROR("Could not initialize depth resources");
            return false;
        }
        if (!initializeFramebuffers()) {
            BL_LOG_ERROR("Could not recreate framebuffers");
            return false;
        }
        return true;
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

    bool Renderer::initializeDepthResources() {
        // VkFormat depthFormat = findDepthFormat();
        // VkExtent2D swapChainExtent = swapChain->getExtent();
        //
        // VulkanImageInfo depthImageConfig{};
        // depthImageConfig.width = swapChainExtent.width;
        // depthImageConfig.height = swapChainExtent.height;
        // depthImageConfig.format = depthFormat;
        // depthImageConfig.tiling = VK_IMAGE_TILING_OPTIMAL;
        // depthImageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        // depthImageConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        // depthImageConfig.mipLevels = 1;
        // depthImageConfig.sampleCount = VK_SAMPLE_COUNT_1_BIT;
        //
        // if (!depthImage->initialize(depthImageConfig)) {
        //     BL_LOG_ERROR("Could not initialize depth image");
        //     return false;
        // }
        //
        // VkImageViewCreateInfo viewInfo{};
        // viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        // viewInfo.image = *depthImage;
        // viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        // viewInfo.format = depthFormat;
        // viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        // viewInfo.subresourceRange.baseMipLevel = 0;
        // viewInfo.subresourceRange.levelCount = 1;
        // viewInfo.subresourceRange.baseArrayLayer = 0;
        // viewInfo.subresourceRange.layerCount = 1;
        //
        // if (device->createImageView(&viewInfo, &depthImageView) != VK_SUCCESS) {
        //     BL_LOG_ERROR("Could not create image view");
        //     return false;
        // }
        //
        // transitionImageLayout(
        //     *depthImage,
        //     depthFormat,
        //     VK_IMAGE_LAYOUT_UNDEFINED,
        //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        // );

        return true;
    }

    bool Renderer::initializeTextureImage() {
        // Image image = fileSystem->readImage("textures/sculpture.png");
        //
        // VulkanBufferConfig stagingBufferConfig{};
        // stagingBufferConfig.size = image.size;
        // stagingBufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        // stagingBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        //
        // auto* stagingBuffer = new VulkanBuffer(commandPool, device, physicalDevice);
        // stagingBuffer->initialize(stagingBufferConfig);
        // stagingBuffer->setData(image.pixels);
        //
        // image.free();
        //
        // VulkanImageInfo textureImageConfig{};
        // textureImageConfig.width = image.width;
        // textureImageConfig.height = image.height;
        // textureImageConfig.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        // textureImageConfig.format = VK_FORMAT_R8G8B8A8_SRGB;
        // textureImageConfig.tiling = VK_IMAGE_TILING_OPTIMAL;
        // textureImageConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        // textureImageConfig.layout = VK_IMAGE_LAYOUT_UNDEFINED;
        // textureImageConfig.mipLevels = 1;
        // textureImageConfig.sampleCount = VK_SAMPLE_COUNT_1_BIT;
        //
        // if (!textureImage->initialize(textureImageConfig)) {
        //     BL_LOG_ERROR("Could not initialize texture image");
        //     return false;
        // }
        //
        // transitionImageLayout(
        //     *textureImage,
        //     VK_FORMAT_R8G8B8A8_SRGB,
        //     VK_IMAGE_LAYOUT_UNDEFINED,
        //     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        // );
        //
        // copyBufferToImage(
        //     *stagingBuffer,
        //     *textureImage,
        //     static_cast<uint32_t>(image.width),
        //     static_cast<uint32_t>(image.height)
        // );
        //
        // transitionImageLayout(
        //     *textureImage,
        //     VK_FORMAT_R8G8B8A8_SRGB,
        //     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        //     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        // );
        //
        // stagingBuffer->terminate();
        //
        // //
        // // IMAGE VIEW
        // //
        //
        // VkImageViewCreateInfo viewInfo{};
        // viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        // viewInfo.image = *textureImage;
        // viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        // viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        // viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // viewInfo.subresourceRange.baseMipLevel = 0;
        // viewInfo.subresourceRange.levelCount = 1;
        // viewInfo.subresourceRange.baseArrayLayer = 0;
        // viewInfo.subresourceRange.layerCount = 1;
        //
        // if (device->createImageView(&viewInfo, &textureImageView) != VK_SUCCESS) {
        //     BL_LOG_ERROR("Could not create image view");
        //     return false;
        // }
        //
        // //
        // // IMAGE SAMPLER
        // //
        //
        // const VkPhysicalDeviceProperties& physicalDeviceProperties = physicalDevice->getProperties();
        //
        // VkSamplerCreateInfo samplerInfo{};
        // samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        // samplerInfo.magFilter = VK_FILTER_LINEAR;
        // samplerInfo.minFilter = VK_FILTER_LINEAR;
        // samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        // samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        // samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        // samplerInfo.anisotropyEnable = VK_TRUE;
        // samplerInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;
        // samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        // samplerInfo.unnormalizedCoordinates = VK_FALSE;
        // samplerInfo.compareEnable = VK_FALSE;
        // samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        // samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        // samplerInfo.mipLodBias = 0.0f;
        // samplerInfo.minLod = 0.0f;
        // samplerInfo.maxLod = 0.0f;
        //
        // if (device->createSampler(&samplerInfo, &textureSampler) != VK_SUCCESS) {
        //     BL_LOG_ERROR("Could not create image sampler");
        //     return false;
        // }

        return true;
    }

    bool Renderer::initializeFramebuffers() {
        const VkExtent2D& swapChainExtent = swapChain->getExtent();
        const std::vector<VkImageView>& swapChainImageViews = swapChain->getImageViews();
        framebuffers.resize(swapChainImageViews.size());
        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            std::array<VkImageView, 2> attachments = {
                swapChainImageViews[i],
                depthImageView
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = *renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (device->createFramebuffer(&framebufferInfo, &framebuffers[i]) != VK_SUCCESS) {
                BL_LOG_ERROR("Could not create Vulkan framebuffer");
                return false;
            }
        }
        return true;
    }

    bool Renderer::initializeGraphicsPipelineObjects() const {
        if (!vertexShader->initialize(fileSystem->readBytes("shaders/shader.vert.spv"))) {
            BL_LOG_ERROR("Could not initialize vertex shader");
            return false;
        }
        if (!fragmentShader->initialize(fileSystem->readBytes("shaders/shader.frag.spv"))) {
            BL_LOG_ERROR("Could not initialize fragment shader");
            return false;
        }
        if (!graphicsPipeline->initialize(descriptorSetLayout)) {
            BL_LOG_ERROR("Could not initialize graphics pipeline");
            return false;
        }
        return true;
    }

    void Renderer::terminateGraphicsPipelineObjects() const {
        graphicsPipeline->terminate();
        fragmentShader->terminate();
        vertexShader->terminate();
    }

    void Renderer::terminateFramebuffers() {
        for (VkFramebuffer framebuffer : framebuffers) {
            device->destroyFramebuffer(framebuffer);
        }
    }
}

