#pragma once

#include "VulkanCommandPool.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
#include "Quad.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <string>

namespace Blink {

    class Renderer {
    private:
        static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        static const uint32_t VERTICES_PER_QUAD = 4;
        static const uint32_t INDICES_PER_QUAD = 6;
        static const uint32_t QUADS_PER_BATCH = 1000;
        static const uint32_t VERTICES_PER_BATCH = QUADS_PER_BATCH * VERTICES_PER_QUAD;
        static const uint32_t INDICES_PER_BATCH = QUADS_PER_BATCH * INDICES_PER_QUAD;
        static const uint32_t MAX_TEXTURE_SLOTS = 16;

    private:
        VulkanCommandPool* commandPool;
        VulkanGraphicsPipeline* graphicsPipeline;
        VulkanRenderPass* renderPass;
        VulkanSwapChain* swapChain;
        VulkanDevice* device;
        VulkanPhysicalDevice* physicalDevice;
        Window* window;
        std::vector<VkFramebuffer> framebuffers;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        uint32_t currentFrame = 0;
        bool framebufferResized = false;

    public:
        Renderer(
                VulkanCommandPool* commandPool,
                VulkanGraphicsPipeline* graphicsPipeline,
                VulkanRenderPass* renderPass,
                VulkanSwapChain* swapChain,
                VulkanDevice* device,
                VulkanPhysicalDevice* physicalDevice,
                Window* window
        );

        bool initialize();

        void terminate();

        void onResize(uint32_t width, uint32_t height);

        void onMinimize(bool minimized);

        void onRender();

        void onComplete();

        void submitQuad(Quad& quad);

    private:
        bool initializeFramebuffers();

        void terminateFramebuffers();

        bool initializeCommandBuffers();

        bool initializeSyncObjects();

        void terminateSyncObjects();

        void drawFrame();

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

        bool recreateSwapChain();

        bool initializeSwapChain();

        void terminateSwapChain();
    };
}


