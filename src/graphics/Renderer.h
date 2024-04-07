#pragma once

#include "system/FileSystem.h"
#include "window/Window.h"
#include "graphics/VulkanApp.h"
#include "graphics/VulkanPhysicalDevice.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanCommandPool.h"
#include "graphics/VulkanPhysicalDevice.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanSwapChain.h"
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
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;

    private:
        FileSystem* fileSystem;
        Window* window;
        VulkanApp* vulkanApp = nullptr;
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VulkanDevice* device = nullptr;
        VulkanSwapChain* swapChain = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        VulkanShader* vertexShader = nullptr;
        VulkanShader* fragmentShader = nullptr;
        VulkanGraphicsPipeline* graphicsPipeline = nullptr;
        VulkanVertexBuffer* vertexBuffer = nullptr;
        VulkanIndexBuffer* indexBuffer = nullptr;
        std::vector<VulkanCommandBuffer> commandBuffers;
        std::vector<VulkanUniformBuffer*> uniformBuffers;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;
        uint32_t currentFrame = 0;

    private:
        VulkanImage* textureImage;
        VkSampler textureSampler;
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

        void render(const Frame& frame) noexcept(false);

        void waitUntilIdle() const noexcept(false);

        void onEvent(Event& event);

    private:
        void setUniformData(VulkanUniformBuffer* uniformBuffer, const Frame& frame) const;

        void bindDescriptorSet(VkDescriptorSet descriptorSet, const VulkanCommandBuffer& commandBuffer) const;

        void drawIndexed(const VulkanCommandBuffer& commandBuffer) const;

        void recompileShaders();

        void compileShaders() const;

        void createGraphicsPipelineObjects();

        void destroyGraphicsPipelineObjects() const;
    };
}
