#pragma once

#include "system/FileSystem.h"
#include "window/Window.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandPool.h"
#include "VulkanShader.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanUniformBuffer.h"
#include "Quad.h"
#include "Vertex.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Blink {

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
        VulkanPhysicalDevice* physicalDevice;
        VulkanDevice* device;
        VulkanSwapChain* swapChain;
        VulkanRenderPass* renderPass;
        VulkanCommandPool* commandPool;
        VulkanShader* vertexShader;
        VulkanShader* fragmentShader;
        VulkanGraphicsPipeline* graphicsPipeline;
        VulkanVertexBuffer* vertexBuffer;
        VulkanIndexBuffer* indexBuffer;
        std::vector<VulkanUniformBuffer*> uniformBuffers;
        std::vector<VkFramebuffer> framebuffers;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;
        uint32_t currentFrame = 0;
        bool framebufferResized = false;
        glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 2.0f);
        glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        float cameraAngleInDegrees = 0.0f;
        glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    private:
        const std::vector<Vertex> vertices = {
                {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
        };
        const std::vector<uint16_t> indices = {
                0, 1, 2, 2, 3, 0
        };

    public:
        Renderer(
            FileSystem* fileSystem,
            Window* window,
            VulkanPhysicalDevice* physicalDevice,
            VulkanDevice* device,
            VulkanSwapChain* swapChain,
            VulkanRenderPass* renderPass,
            VulkanCommandPool* commandPool
        );

        virtual ~Renderer();

        bool initialize();

        void terminate();

        void onResize(uint32_t width, uint32_t height);

        void onMinimize(bool minimized);

        void onRender();

        void onComplete();

        void submitQuad(Quad& quad);

    private:
        bool initializeDescriptorObjects();

        void terminateDescriptorObjects();

        bool initializeUniformBuffers();

        void terminateUniformBuffers();

        bool initializeFramebuffers();

        void terminateFramebuffers();

        bool initializeCommandBuffers();

        bool initializeSyncObjects();

        void terminateSyncObjects();

        bool recreateSwapChain();

        bool initializeSwapChain();

        void terminateSwapChain();

        void drawFrame();

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

        void updateUniformBuffer(VulkanUniformBuffer* uniformBuffer);
    };
}


