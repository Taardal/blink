#include "VulkanSwapChain.h"

namespace Blink {

    VulkanSwapChain::VulkanSwapChain(const VulkanSwapChainConfig& config) : config(config) {
        BL_TRY(createSwapChain());
        BL_TRY(createColorImages());
        BL_TRY(createDepthImage());
        BL_TRY(createRenderPass());
        BL_TRY(createFramebuffers());
        BL_TRY(createSyncObjects());
    }

    VulkanSwapChain::~VulkanSwapChain() {
        destroySyncObjects();
        destroyFramebuffers();
        destroyRenderPass();
        destroyDepthImage();
        destroyColorImages();
        destroySwapChain();
        BL_LOG_INFO("Destroyed swap chain");
    }

    VulkanSwapChain::operator VkSwapchainKHR() const {
        return swapChain;
    }

    VkRenderPass VulkanSwapChain::getRenderPass() const {
        return renderPass;
    }

    const VkExtent2D& VulkanSwapChain::getExtent() const {
        return extent;
    }

    void VulkanSwapChain::onEvent(Event& event) {
        if (event.type == EventType::WindowResize || event.type == EventType::WindowMinimize) {
            framebufferResized = true;
        }
    }

    bool VulkanSwapChain::beginFrame(uint32_t frameIndex) noexcept(false) {
        inFlightFence = inFlightFences[frameIndex];
        imageAvailableSemaphore = imageAvailableSemaphores[frameIndex];
        renderFinishedSemaphore = renderFinishedSemaphores[frameIndex];

        BL_ASSERT_THROW_VK_SUCCESS(config.device->waitForFence(&inFlightFence));

        VkResult nextImageResult = config.device->acquireSwapChainImage(swapChain, imageAvailableSemaphore, &imageIndex);
        if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
            recreate();
            return false;
        }
        if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR) {
            BL_THROW("Could not acquire next image from swap chain");
        }

        BL_ASSERT_THROW_VK_SUCCESS(config.device->resetFence(&inFlightFence));

        return true;
    }

    void VulkanSwapChain::beginRenderPass(const VulkanCommandBuffer& commandBuffer) noexcept(false) {
        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = framebuffers[imageIndex];
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = extent;
        renderPassBeginInfo.clearValueCount = (uint32_t) clearValues.size();
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) extent.width;
        viewport.height = (float) extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;

        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VulkanSwapChain::endRenderPass(const VulkanCommandBuffer& commandBuffer) noexcept(false) {
        vkCmdEndRenderPass(commandBuffer);
    }

    void VulkanSwapChain::endFrame(const VulkanCommandBuffer& commandBuffer) {
        VkPipelineStageFlags colorOutputPipelineStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
        submitInfo.pWaitDstStageMask = &colorOutputPipelineStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffer.vk_ptr();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

        if (config.device->submitToGraphicsQueue(&submitInfo, inFlightFence) != VK_SUCCESS) {
            BL_THROW("Could not submit draw command buffer");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapChain;
        presentInfo.pImageIndices = &imageIndex;

        VkResult presentResult = config.device->submitToPresentQueue(&presentInfo);
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreate();
        } else if (presentResult != VK_SUCCESS) {
            BL_THROW("Could not present image to swap chain");
        }
    }

    void VulkanSwapChain::recreate() {
        config.window->waitUntilNotMinimized();
        BL_ASSERT_THROW_VK_SUCCESS(config.device->waitUntilIdle());

        destroyFramebuffers();
        destroyDepthImage();
        destroyColorImages();
        destroySwapChain();

        config.physicalDevice->updateSwapChainInfo();

        BL_TRY(createSwapChain());
        BL_TRY(createColorImages());
        BL_TRY(createDepthImage());
        BL_TRY(createFramebuffers());

        BL_LOG_INFO("Recreated swap chain");
    }

    void VulkanSwapChain::createSwapChain() {
        VkSurfaceKHR surface = config.vulkanApp->getSurface();
        const SwapChainInfo& swapChainInfo = config.physicalDevice->getSwapChainInfo();
        const QueueFamilyIndices& queueFamilyIndices = config.physicalDevice->getQueueFamilyIndices();

        surfaceFormat = getMostSuitableSurfaceFormat(swapChainInfo.surfaceFormats);
        presentMode = getMostSuitablePresentMode(swapChainInfo.presentModes);
        extent = getMostSuitableExtent(swapChainInfo.surfaceCapabilities);
        imageCount = getMostSuitableImageCount(swapChainInfo.surfaceCapabilities);

        BL_ASSERT_THROW(imageCount > 0);

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.minImageCount = imageCount;
        createInfo.surface = surface;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform = swapChainInfo.surfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentFamily) {
            std::vector<uint32_t> queueFamilyIndexValues = {
                    queueFamilyIndices.graphicsFamily.value(),
                    queueFamilyIndices.presentFamily.value()
            };
            createInfo.pQueueFamilyIndices = queueFamilyIndexValues.data();
            createInfo.queueFamilyIndexCount = queueFamilyIndexValues.size();
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        } else {
            createInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createSwapChain(&createInfo, &swapChain));
    }

    void VulkanSwapChain::destroySwapChain() const {
        return config.device->destroySwapChain(swapChain);
    }

    void VulkanSwapChain::createColorImages() {
        BL_ASSERT_THROW_VK_SUCCESS(config.device->getSwapChainImages(swapChain, &imageCount, &colorImages));
        colorImageViews.resize(imageCount);
        for (size_t i = 0; i < imageCount; i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = colorImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = surfaceFormat.format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            BL_ASSERT_THROW_VK_SUCCESS(config.device->createImageView(&createInfo, &colorImageViews[i]));
        }
    }

    void VulkanSwapChain::destroyColorImages() {
        for (VkImageView colorImageView : colorImageViews) {
            config.device->destroyImageView(colorImageView);
        }
        colorImageViews.clear();

        // Color images (VkImage) are destroyed with VkSwapChainKHR object
        colorImages.clear();
    }

    void VulkanSwapChain::createDepthImage() {
        VkFormat depthFormat = config.physicalDevice->getDepthFormat();

        VkImageCreateInfo depthImageCreateInfo{};
        depthImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        depthImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        depthImageCreateInfo.extent.width = extent.width;
        depthImageCreateInfo.extent.height = extent.height;
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
        depthImageConfig.physicalDevice = config.physicalDevice;
        depthImageConfig.device = config.device;
        depthImageConfig.commandPool = config.commandPool;

        BL_TRY(depthImage = new VulkanImage(depthImageConfig));
        BL_TRY(depthImage->setLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));

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

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createImageView(&depthImageViewCreateInfo, &depthImageView));
    }

    void VulkanSwapChain::destroyDepthImage() const {
        config.device->destroyImageView(depthImageView);
        delete depthImage;
    }

    void VulkanSwapChain::createRenderPass() {
        VkAttachmentDescription colorAttachmentDescription{};
        colorAttachmentDescription.format = surfaceFormat.format;
        colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentReference{};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachmentDescription{};
        depthAttachmentDescription.format = config.physicalDevice->getDepthFormat();
        depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentReference{};
        depthAttachmentReference.attachment = 1;
        depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription{};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorAttachmentReference;
        subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

        VkSubpassDependency subpassDependency{};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachmentDescriptions = {
          colorAttachmentDescription,
          depthAttachmentDescription
        };
        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
        renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &subpassDependency;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createRenderPass(&renderPassCreateInfo, &renderPass));

        VkClearColorValue clearColorValue = {
          {0.0f, 0.0f, 0.0f, 1.0f}
        };

        VkClearDepthStencilValue clearDepthStencilValue{};
        clearDepthStencilValue.depth = 1.0f;
        clearDepthStencilValue.stencil = 0;

        clearValues[0].color = clearColorValue;
        clearValues[1].depthStencil = clearDepthStencilValue;
    }

    void VulkanSwapChain::destroyRenderPass() {
        return config.device->destroyRenderPass(renderPass);
    }

    void VulkanSwapChain::createFramebuffers() {
        framebuffers.resize(imageCount);
        for (size_t i = 0; i < imageCount; i++) {
            std::array<VkImageView, 2> attachments = {
                    colorImageViews[i],
                    depthImageView
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = extent.width;
            framebufferInfo.height = extent.height;
            framebufferInfo.layers = 1;

            BL_ASSERT_THROW_VK_SUCCESS(config.device->createFramebuffer(&framebufferInfo, &framebuffers[i]));
        }
    }

    void VulkanSwapChain::destroyFramebuffers() {
        for (VkFramebuffer framebuffer : framebuffers) {
            config.device->destroyFramebuffer(framebuffer);
        }
    }

    void VulkanSwapChain::createSyncObjects() {
        imageAvailableSemaphores.resize(imageCount);
        renderFinishedSemaphores.resize(imageCount);
        inFlightFences.resize(imageCount);

        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < imageCount; i++) {
            BL_ASSERT_THROW_VK_SUCCESS(config.device->createSemaphore(&semaphoreCreateInfo, &imageAvailableSemaphores[i]));
            BL_ASSERT_THROW_VK_SUCCESS(config.device->createSemaphore(&semaphoreCreateInfo, &renderFinishedSemaphores[i]));
            BL_ASSERT_THROW_VK_SUCCESS(config.device->createFence(&fenceCreateInfo, &inFlightFences[i]));
        }
    }

    void VulkanSwapChain::destroySyncObjects() {
        for (size_t i = 0; i < imageCount; i++) {
            config.device->destroySemaphore(imageAvailableSemaphores[i]);
            config.device->destroySemaphore(renderFinishedSemaphores[i]);
            config.device->destroyFence(inFlightFences[i]);
        }
    }

    VkSurfaceFormatKHR VulkanSwapChain::getMostSuitableSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapChain::getMostSuitablePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapChain::getMostSuitableExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const {
        bool extentMustMatchWindowSize = surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max();
        if (extentMustMatchWindowSize) {
            return surfaceCapabilities.currentExtent;
        }
        WindowSize windowSizeInPixels = config.window->getSizeInPixels();
        uint32_t width = std::clamp((uint32_t) windowSizeInPixels.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        uint32_t height = std::clamp((uint32_t) windowSizeInPixels.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        return { width, height };
    }

    uint32_t VulkanSwapChain::getMostSuitableImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const {
        uint32_t minImageCount = surfaceCapabilities.minImageCount;
        uint32_t maxImageCount = surfaceCapabilities.maxImageCount;

        // Simply sticking to the surface capability minimum means that we may sometimes have to wait on the driver to
        // complete internal operations before we can acquire another image to render to.
        // Therefore it is recommended to request at least one more image than the minimum
        uint32_t imageCount = minImageCount + 1;

        if (maxImageCount > 0 && imageCount > maxImageCount) {
            imageCount = maxImageCount;
        }
        return imageCount;
    }
}
