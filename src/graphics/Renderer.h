#pragma once

#include "system/FileSystem.h"
#include "window/Window.h"
#include "graphics/VulkanApp.h"
#include "graphics/VulkanPhysicalDevice.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanCommandPool.h"
#include "graphics/VulkanPhysicalDevice.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanCommandPool.h"
#include "graphics/VulkanShader.h"
#include "graphics/VulkanGraphicsPipeline.h"
#include "graphics/VulkanVertexBuffer.h"
#include "graphics/VulkanIndexBuffer.h"
#include "graphics/VulkanUniformBuffer.h"
#include "graphics/VulkanImage.h"
#include "graphics/Quad.h"
#include "graphics/Vertex.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Blink {
    struct Frame {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };

    class Renderer {
    private:
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        static constexpr uint32_t VERTICES_PER_QUAD = 4;
        static constexpr uint32_t INDICES_PER_QUAD = 6;
        static constexpr uint32_t QUADS_PER_BATCH = 1000;
        static constexpr uint32_t VERTICES_PER_BATCH = QUADS_PER_BATCH * VERTICES_PER_QUAD;
        static constexpr uint32_t INDICES_PER_BATCH = QUADS_PER_BATCH * INDICES_PER_QUAD;
        static constexpr uint32_t MAX_TEXTURE_SLOTS = 16;

    private:
        FileSystem* fileSystem;
        Window* window;
        VulkanApp* vulkanApp = VK_NULL_HANDLE;
        VulkanPhysicalDevice* physicalDevice = VK_NULL_HANDLE;
        VulkanDevice* device = VK_NULL_HANDLE;
        VulkanSwapChain* swapChain = VK_NULL_HANDLE;
        VulkanRenderPass* renderPass = VK_NULL_HANDLE;
        VulkanCommandPool* commandPool = VK_NULL_HANDLE;
        VulkanShader* vertexShader = VK_NULL_HANDLE;
        VulkanShader* fragmentShader = VK_NULL_HANDLE;
        VulkanGraphicsPipeline* graphicsPipeline = VK_NULL_HANDLE;
        VulkanVertexBuffer* vertexBuffer = VK_NULL_HANDLE;
        VulkanIndexBuffer* indexBuffer = VK_NULL_HANDLE;
        std::vector<VulkanUniformBuffer*> uniformBuffers;
        std::vector<VkFramebuffer> framebuffers;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> descriptorSets;
        uint32_t currentFrame = 0;
        bool framebufferResized = false;

        VulkanImage* textureImage;
        VkImageView textureImageView;
        VkSampler textureSampler;

        VulkanImage* depthImage;
        VkImageView depthImageView;

    private:
        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

            {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
        };

        const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4
        };

    public:
        Renderer(
            const AppConfig& appConfig,
            FileSystem* fileSystem,
            Window* window
        ) noexcept(false);

        ~Renderer();

        void onEvent(Event& event);

        void render(const Frame& frame);

    private:
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) const;

        void updateUniformBuffer(VulkanUniformBuffer* uniformBuffer, const Frame& frame);

        bool recreateSwapChain();

        void compileShaders();

        void terminate();

        bool initializeUniformBuffers();

        void terminateUniformBuffers() const;

        bool initializeGraphicsPipelineObjects() const;

        void terminateGraphicsPipelineObjects() const;

        bool initializeDescriptorObjects();

        void terminateDescriptorObjects() const;

        bool initializeFramebuffers();

        void terminateFramebuffers();

        bool initializeCommandBuffers();

        bool initializeSyncObjects();

        void terminateSyncObjects() const;

        bool initializeTextureImage();

        bool createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const;

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;

        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;

        VkCommandBuffer beginSingleTimeCommands() const;

        void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

        bool initializeDepthResources();

        VkFormat findDepthFormat() const;

        bool hasStencilComponent(VkFormat format) const;
    };
}
