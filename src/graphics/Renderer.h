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

    private:
        const std::vector<Vertex> vertices = {
            // Front face
            {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},

            // Back face
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
        };
        const std::vector<uint16_t> indices = {
            // Front face
            0, 1, 2, 2, 3, 0,
            // Right face
            1, 5, 6, 6, 2, 1,
            // Back face
            5, 4, 7, 7, 6, 5,
            // Left face
            4, 0, 3, 3, 7, 4,
            // Top face
            3, 2, 6, 6, 7, 3,
            // Bottom face
            0, 1, 5, 5, 4, 0
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

        ~Renderer();

        void onEvent(Event& event);

        void render(const Frame& frame);

    private:
        bool initialize();

        void terminate();

        bool initializeUniformBuffers();

        void terminateUniformBuffers() const;

        bool initializeDescriptorObjects();

        void terminateDescriptorObjects() const;

        bool initializeGraphicsPipelineObjects() const;

        void terminateGraphicsPipelineObjects() const;

        bool initializeFramebuffers();

        void terminateFramebuffers();

        bool initializeCommandBuffers();

        bool initializeSyncObjects();

        void terminateSyncObjects() const;

        bool recreateSwapChain();

        bool initializeSwapChain();

        void terminateSwapChain();

        void drawFrame(const Frame& frame);

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) const;

        static void updateUniformBuffer(VulkanUniformBuffer* uniformBuffer, const Frame& frame);

        static void compileShaders();
    };
}


