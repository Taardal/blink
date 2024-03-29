#include "pch.h"
#include "Renderer.h"
#include "UniformBufferObject.h"
#include "window/KeyEvent.h"

#include <chrono>

#include "VulkanImage.h"

namespace Blink {

    Renderer::Renderer(
        const AppConfig& appConfig,
        FileSystem* fileSystem,
        Window* window
    ) : fileSystem(fileSystem),
        window(window),
        vulkanApp(new VulkanApp(appConfig, window)),
        physicalDevice(new VulkanPhysicalDevice(vulkanApp)),
        device(new VulkanDevice(physicalDevice)),
        swapChain(new VulkanSwapChain(device, physicalDevice, vulkanApp, window)),
        renderPass(new VulkanRenderPass(swapChain, device, physicalDevice)),
        commandPool(new VulkanCommandPool(device, physicalDevice)),
        vertexShader(new VulkanShader(device)),
        fragmentShader(new VulkanShader(device)),
        graphicsPipeline(new VulkanGraphicsPipeline(vertexShader, fragmentShader, renderPass, swapChain, device)),
        vertexBuffer(new VulkanVertexBuffer(commandPool, device, physicalDevice)),
        indexBuffer(new VulkanIndexBuffer(commandPool, device, physicalDevice)),
        textureImage(new VulkanImage(device, physicalDevice)),
        depthImage(new VulkanImage(device, physicalDevice))
    {
        if (!initialize()) {
            throw std::runtime_error("Could not initialize renderer");
        }
    }

    bool Renderer::initialize() {
        if (!initializeUniformBuffers()) {
            BL_LOG_ERROR("Could not initialize uniform buffers");
            return false;
        }
        if (!initializeDepthResources()) {
            BL_LOG_ERROR("Could not initialize depth resources");
            return false;
        }
        if (!initializeTextureImage()) {
            BL_LOG_ERROR("Could not initialize texture image");
            return false;
        }
        if (!initializeDescriptorObjects()) {
            BL_LOG_ERROR("Could not initialize descriptor objects");
            return false;
        }
        if (!initializeGraphicsPipelineObjects()) {
            BL_LOG_ERROR("Could not initialize graphics pipeline objects");
            return false;
        }
        if (!vertexBuffer->initialize(vertices)) {
            BL_LOG_ERROR("Could not initialize vertex buffer");
            return false;
        }
        if (!indexBuffer->initialize(indices)) {
            BL_LOG_ERROR("Could not initialize vertex buffer");
            return false;
        }
        if (!initializeFramebuffers()) {
            BL_LOG_ERROR("Could not initialize framebuffers");
            return false;
        }
        if (!initializeCommandBuffers()) {
            BL_LOG_ERROR("Could not initialize command buffers");
            return false;
        }
        if (!initializeSyncObjects()) {
            BL_LOG_ERROR("Could not initialize sync objects");
            return false;
        }
        return true;
    }

    bool Renderer::initializeDepthResources() {
        VkFormat depthFormat = findDepthFormat();
        VkExtent2D swapChainExtent = swapChain->getExtent();

        VulkanImageConfig depthImageConfig{};
        depthImageConfig.width = swapChainExtent.width;
        depthImageConfig.height = swapChainExtent.height;
        depthImageConfig.format = depthFormat;
        depthImageConfig.tiling = VK_IMAGE_TILING_OPTIMAL;
        depthImageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthImageConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        depthImageConfig.mipLevels = 1;
        depthImageConfig.sampleCount = VK_SAMPLE_COUNT_1_BIT;

        if (!depthImage->initialize(depthImageConfig)) {
            BL_LOG_ERROR("Could not initialize depth image");
            return false;
        }

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = *depthImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = depthFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (device->createImageView(&viewInfo, &depthImageView) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create image view");
            return false;
        }

        transitionImageLayout(
            *depthImage,
            depthFormat,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        );

        return true;
    }

    VkFormat Renderer::findDepthFormat() const {
        std::vector<VkFormat> candidates = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        };
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
        VkFormatFeatureFlagBits features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        return physicalDevice->findSupportedFormat(candidates, tiling, features);
    }

    bool Renderer::hasStencilComponent(VkFormat format) const {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    bool Renderer::initializeTextureImage() {
        Image image = fileSystem->readImage("textures/sculpture.png");

        VulkanBufferConfig stagingBufferConfig{};
        stagingBufferConfig.size = image.size;
        stagingBufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        auto* stagingBuffer = new VulkanBuffer(commandPool, device, physicalDevice);
        stagingBuffer->initialize(stagingBufferConfig);
        stagingBuffer->setData(image.pixels);

        image.free();

        VulkanImageConfig textureImageConfig{};
        textureImageConfig.width = image.width;
        textureImageConfig.height = image.height;
        textureImageConfig.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureImageConfig.format = VK_FORMAT_R8G8B8A8_SRGB;
        textureImageConfig.tiling = VK_IMAGE_TILING_OPTIMAL;
        textureImageConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        textureImageConfig.layout = VK_IMAGE_LAYOUT_UNDEFINED;
        textureImageConfig.mipLevels = 1;
        textureImageConfig.sampleCount = VK_SAMPLE_COUNT_1_BIT;

        if (!textureImage->initialize(textureImageConfig)) {
            BL_LOG_ERROR("Could not initialize texture image");
            return false;
        }

        transitionImageLayout(
            *textureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        );

        copyBufferToImage(
            *stagingBuffer,
            *textureImage,
            static_cast<uint32_t>(image.width),
            static_cast<uint32_t>(image.height)
        );

        transitionImageLayout(
            *textureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );

        stagingBuffer->terminate();

        //
        // IMAGE VIEW
        //

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = *textureImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (device->createImageView(&viewInfo, &textureImageView) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create image view");
            return false;
        }

        //
        // IMAGE SAMPLER
        //

        const VkPhysicalDeviceProperties& physicalDeviceProperties = physicalDevice->getProperties();

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (device->createSampler(&samplerInfo, &textureSampler) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create image sampler");
            return false;
        }

        return true;
    }

    void Renderer::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(format)) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            throw std::invalid_argument(BL_TAG("Unsupported layout transition"));
        }

        constexpr VkDependencyFlags dependencyFlags = 0;
        constexpr uint32_t memoryBarrierCount = 0;
        constexpr VkMemoryBarrier* memoryBarriers = VK_NULL_HANDLE;
        constexpr uint32_t bufferMemoryBarrierCount = 0;
        constexpr VkBufferMemoryBarrier* bufferMemoryBarriers = VK_NULL_HANDLE;
        constexpr uint32_t imageMemoryBarrierCount = 1;
        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage,
            destinationStage,
            dependencyFlags,
            memoryBarrierCount,
            memoryBarriers,
            bufferMemoryBarrierCount,
            bufferMemoryBarriers,
            imageMemoryBarrierCount,
            &barrier
        );

        endSingleTimeCommands(commandBuffer);
    }

    void Renderer::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        constexpr uint32_t depth = 1;
        VkExtent3D imageExtent = {
            width,
            height,
            depth
        };

        constexpr int32_t x = 0;
        constexpr int32_t y = 0;
        constexpr int32_t z = 0;
        VkOffset3D imageOffset = { x, y, z };

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = imageOffset;
        region.imageExtent = imageExtent;

        constexpr uint32_t regionCount = 1;
        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            regionCount,
            &region
        );

        endSingleTimeCommands(commandBuffer);
    }

    VkCommandBuffer Renderer::beginSingleTimeCommands() const {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = *commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(*device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void Renderer::endSingleTimeCommands(VkCommandBuffer commandBuffer) const {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(device->getGraphicsQueue());

        vkFreeCommandBuffers(*device, *commandPool, 1, &commandBuffer);
    }

    void Renderer::terminate() {
        device->destroyImageView(depthImageView);
        depthImage->terminate();

        device->destroySampler(textureSampler);
        device->destroyImageView(textureImageView);
        textureImage->terminate();

        terminateSyncObjects();
        terminateFramebuffers();
        indexBuffer->terminate();
        vertexBuffer->terminate();
        terminateGraphicsPipelineObjects();
        terminateDescriptorObjects();
        terminateUniformBuffers();
    }

    Renderer::~Renderer() {
        device->waitUntilIdle();
        terminate();
        for (VulkanUniformBuffer* uniformBuffer : uniformBuffers) {
            delete uniformBuffer;
        }
        delete indexBuffer;
        delete vertexBuffer;
        delete graphicsPipeline;
        delete fragmentShader;
        delete vertexShader;
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
            throw std::runtime_error("Could not acquire next image from swap chain");
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
            throw std::runtime_error("Could not submit draw command buffer");
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
            throw std::runtime_error("Could not present image to swap chain");
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
            throw std::runtime_error("Could not begin recording command buffer");
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
            throw std::runtime_error("Could not end frame");
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
        depthImage->terminate();
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

    bool Renderer::initializeUniformBuffers() {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            auto uniformBuffer = new VulkanUniformBuffer(commandPool, device, physicalDevice);
            if (!uniformBuffer->initialize(sizeof(UniformBufferObject))) {
                BL_LOG_ERROR("Could not initialize uniform buffer for frame [{}]", i);
                return false;
            }
            uniformBuffers.push_back(uniformBuffer);
        }
        return true;
    }

    void Renderer::terminateUniformBuffers() const {
        for (VulkanUniformBuffer* uniformBuffer : uniformBuffers) {
            uniformBuffer->terminate();
        }
    }

    bool Renderer::initializeDescriptorObjects() {

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

        if (device->createDescriptorSetLayout(&layoutInfo, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error(BL_TAG("Could not create descriptor set layout"));
        }

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

        if (device->createDescriptorPool(&poolInfo, &descriptorPool) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create descrptor pool");
            return false;
        }

        //
        // DESCRIPTOR SETS
        //

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

        descriptorSets.resize(descriptorSetAllocateInfo.descriptorSetCount);
        if (device->allocateDescriptorSets(&descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not allocate descriptor sets");
            return false;
        }

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
        return true;
    }

    void Renderer::terminateDescriptorObjects() const {
        device->destroyDescriptorPool(descriptorPool);
        device->destroyDescriptorSetLayout(descriptorSetLayout);
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

    bool Renderer::initializeCommandBuffers() {
        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        return commandPool->allocateCommandBuffers(&commandBuffers);
    }

    bool Renderer::initializeSyncObjects() {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (device->createSemaphore(&semaphoreCreateInfo, &imageAvailableSemaphores[i]) != VK_SUCCESS) {
                BL_LOG_ERROR("Could not create 'image available' semaphore for frame [{}]", 0);
                return false;
            }
            if (device->createSemaphore(&semaphoreCreateInfo, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
                BL_LOG_ERROR("Could not create 'render finished' semaphore for frame [{}]", 0);
                return false;
            }
            if (device->createFence(&fenceCreateInfo, &inFlightFences[i]) != VK_SUCCESS) {
                BL_LOG_ERROR("Could not create 'in flight' fence for frame [{}]", 0);
                return false;
            }
        }
        return true;
    }

    void Renderer::terminateSyncObjects() const {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            device->destroySemaphore(imageAvailableSemaphores[i]);
            device->destroySemaphore(renderFinishedSemaphores[i]);
            device->destroyFence(inFlightFences[i]);
        }
    }
}

