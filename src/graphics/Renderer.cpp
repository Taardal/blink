#include "Renderer.h"

#include "UniformBufferObject.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

namespace Blink {

    Renderer::Renderer(
        FileSystem* fileSystem,
        Window* window,
        VulkanPhysicalDevice* physicalDevice,
        VulkanDevice* device,
        VulkanSwapChain* swapChain,
        VulkanRenderPass* renderPass,
        VulkanCommandPool* commandPool
    ) : fileSystem(fileSystem),
        window(window),
        physicalDevice(physicalDevice),
        device(device),
        swapChain(swapChain),
        renderPass(renderPass),
        commandPool(commandPool),
        vertexShader(new VulkanShader(device)),
        fragmentShader(new VulkanShader(device)),
        graphicsPipeline(new VulkanGraphicsPipeline(vertexShader, fragmentShader, renderPass, swapChain, device)),
        vertexBuffer(new VulkanVertexBuffer(commandPool, device, physicalDevice)),
        indexBuffer(new VulkanIndexBuffer(commandPool, device, physicalDevice))
    {
        initialize();
    }

    bool Renderer::initialize() {
        window->setResizeListener([this](uint32_t width, uint32_t height) {
            framebufferResized = true;
        });
        window->setMinimizeListener([this](bool minimized) {
            framebufferResized = true;
        });
        if (!vertexShader->initialize(fileSystem->readBytes("shaders/shader.vert.spv"))) {
            BL_LOG_ERROR("Could not initialize vertex shader");
            return false;
        }
        if (!fragmentShader->initialize(fileSystem->readBytes("shaders/shader.frag.spv"))) {
            BL_LOG_ERROR("Could not initialize fragment shader");
            return false;
        }
        if (!initializeUniformBuffers()) {
            BL_LOG_ERROR("Could not initialize uniform buffers");
            return false;
        }
        if (!initializeDescriptorObjects()) {
            BL_LOG_ERROR("Could not initialize descriptor objects");
            return false;
        }
        if (!graphicsPipeline->initialize(descriptorSetLayout)) {
            BL_LOG_ERROR("Could not initialize graphics pipeline");
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

    void Renderer::terminate() {
        device->waitUntilIdle();
        terminateSyncObjects();
        terminateFramebuffers();
        indexBuffer->terminate();
        vertexBuffer->terminate();
        graphicsPipeline->terminate();
        terminateDescriptorObjects();
        terminateUniformBuffers();
        fragmentShader->terminate();
        vertexShader->terminate();
    }

    Renderer::~Renderer() {
        terminate();
        for (VulkanUniformBuffer* uniformBuffer : uniformBuffers) {
            delete uniformBuffer;
        }
        delete indexBuffer;
        delete vertexBuffer;
        delete graphicsPipeline;
        delete fragmentShader;
        delete vertexShader;
    }

    void Renderer::onResize(uint32_t width, uint32_t height) {
        this->framebufferResized = true;
    };

    void Renderer::onMinimize(bool minimized) {
        this->framebufferResized = true;
    };

    void Renderer::render(const Frame& frame) {
        drawFrame(frame);
    }

    void Renderer::submitQuad(Quad& quad) {
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

    bool Renderer::initializeDescriptorObjects() {

        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = 1;
        descriptorSetLayoutCreateInfo.pBindings = &uboLayoutBinding;

        if (device->createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, &descriptorSetLayout) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create descrptor set layout");
            return false;
        }

        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = 1;
        descriptorPoolCreateInfo.pPoolSizes = &poolSize;
        descriptorPoolCreateInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

        if (device->createDescriptorPool(&descriptorPoolCreateInfo, &descriptorPool) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create descrptor pool");
            return false;
        }

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (device->allocateDescriptorSets(&descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not allocate descriptor sets");
            return false;
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i]->getBuffer()->getBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;

            device->updateDescriptorSets(1, &descriptorWrite);
        }
        return true;
    }

    void Renderer::terminateDescriptorObjects() {
        device->destroyDescriptorPool(descriptorPool);
        device->destroyDescriptorSetLayout(descriptorSetLayout);
    }

    void Renderer::terminateUniformBuffers() {
        for (VulkanUniformBuffer* uniformBuffer : uniformBuffers) {
            uniformBuffer->terminate();
        }
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

    void Renderer::drawFrame(const Frame& frame) {
        /*
         * Frame resources
         */

        VkFence inFlightFence = inFlightFences[currentFrame];
        VkSemaphore imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
        VkSemaphore renderFinishedSemaphore = renderFinishedSemaphores[currentFrame];
        VkCommandBuffer commandBuffer = commandBuffers[currentFrame];
        VulkanUniformBuffer* uniformBuffer = uniformBuffers[currentFrame];

        /*
         * Acquisition
         */

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

        /*
         * Recording & Submission
         */

        recordCommandBuffer(commandBuffer, imageIndex);
        updateUniformBuffer(uniformBuffer, frame);

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

    void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
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
}

