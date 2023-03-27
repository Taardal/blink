#include "Renderer.h"

namespace Blink {

    Renderer::Renderer(
            VulkanCommandPool* commandPool,
            VulkanGraphicsPipeline* graphicsPipeline,
            VulkanRenderPass* renderPass,
            VulkanSwapChain* swapChain,
            VulkanDevice* device,
            VulkanPhysicalDevice* physicalDevice,
            Window* window
    ) : commandPool(commandPool),
        graphicsPipeline(graphicsPipeline),
        renderPass(renderPass),
        swapChain(swapChain),
        device(device),
        physicalDevice(physicalDevice),
        window(window) {}

    bool Renderer::initialize() {
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

    void Renderer::terminate() {
        terminateSyncObjects();
        terminateFramebuffers();
    }

    void Renderer::onResize(uint32_t width, uint32_t height) {
        this->framebufferResized = true;
    };

    void Renderer::onMinimize(bool minimized) {
        this->framebufferResized = true;
    };

    void Renderer::onRender() {
        drawFrame();
    }

    void Renderer::onComplete() {
        device->waitUntilIdle();
    }

    void Renderer::submitQuad(Quad& quad) {
    }

    bool Renderer::initializeFramebuffers() {
        const std::vector<VkImageView>& swapChainImageViews = swapChain->getImageViews();
        framebuffers.resize(swapChainImageViews.size());
        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            VkFramebufferCreateInfo framebufferCreateInfo{};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = renderPass->getRenderPass();
            framebufferCreateInfo.layers = 1;

            VkImageView attachments[] = {
                    swapChainImageViews[i]
            };
            framebufferCreateInfo.pAttachments = attachments;
            framebufferCreateInfo.attachmentCount = 1;

            const VkExtent2D& swapChainExtent = swapChain->getExtent();
            framebufferCreateInfo.width = swapChainExtent.width;
            framebufferCreateInfo.height = swapChainExtent.height;

            if (device->createFramebuffer(&framebufferCreateInfo, &framebuffers[i]) != VK_SUCCESS) {
                BL_LOG_ERROR("Could not create Vulkan framebuffer");
                return false;
            }
        }
        return true;
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

    void Renderer::terminateSyncObjects() {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            device->destroySemaphore(imageAvailableSemaphores[i]);
            device->destroySemaphore(renderFinishedSemaphores[i]);
            device->destroyFence(inFlightFences[i]);
        }
    }

    void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Could not begin recording command buffer");
        }

        renderPass->begin(commandBuffer, framebuffers.at(imageIndex));
        graphicsPipeline->bind(commandBuffer);

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
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        renderPass->end(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Could not end frame");
        }
    }

    void Renderer::drawFrame() {
        /*
         * Acquisition
         */

        device->waitForFence(&inFlightFences[currentFrame]);

        uint32_t imageIndex;
        VkResult nextImageResult = swapChain->acquireNextImage(imageAvailableSemaphores[currentFrame], &imageIndex);
        if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Could not acquire next image from swap chain");
        }

        device->resetFence(&inFlightFences[currentFrame]);

        VkCommandBufferResetFlags commandBufferResetFlags = 0;
        vkResetCommandBuffer(commandBuffers[currentFrame], commandBufferResetFlags);

        /*
         * Recording & Submission
         */

        recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {
                imageAvailableSemaphores[currentFrame]
        };
        VkPipelineStageFlags waitStages[] = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = {
                renderFinishedSemaphores[currentFrame]
        };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        uint32_t submitCount = 1;
        if (vkQueueSubmit(device->getGraphicsQueue(), submitCount, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("Could not submit draw command buffer");
        }

        /*
         * Presentation
         */
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {
                swapChain->getSwapChain()
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

        /*
         * Next frame
         */
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    bool Renderer::recreateSwapChain() {
        window->waitUntilNotMinimized();
        device->waitUntilIdle();
        terminateSwapChain();
        physicalDevice->updateSwapChainInfo();
        return initializeSwapChain();
    }

    bool Renderer::initializeSwapChain() {
        if (!swapChain->initialize()) {
            BL_LOG_ERROR("Could not initialize swap chain");
            return false;
        }
        if (!initializeFramebuffers()) {
            BL_LOG_ERROR("Could not initialize framebuffers");
            return false;
        }
        return true;
    }

    void Renderer::terminateSwapChain() {
        terminateFramebuffers();
        swapChain->terminate();
    }
}

