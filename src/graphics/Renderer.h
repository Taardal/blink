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

    struct RendererConfig {
        std::string applicationName;
        std::string engineName;
        FileSystem* fileSystem = nullptr;
        Window* window = nullptr;
    };

    class Renderer {
    private:
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;

    private:
        RendererConfig config;
        VulkanApp* vulkanApp = nullptr;
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        std::vector<VulkanCommandBuffer> commandBuffers;
        VulkanSwapChain* swapChain = nullptr;
        VulkanVertexBuffer* vertexBuffer = nullptr;
        VulkanIndexBuffer* indexBuffer = nullptr;
        std::vector<VulkanUniformBuffer*> uniformBuffers;
        VulkanImage* textureImage;
        VkSampler textureSampler;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;
        VulkanShader* vertexShader = nullptr;
        VulkanShader* fragmentShader = nullptr;
        VulkanGraphicsPipeline* graphicsPipeline = nullptr;
        uint32_t currentFrame = 0;

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
        explicit Renderer(const RendererConfig& config) noexcept(false);

        ~Renderer();

        void waitUntilIdle() const noexcept(false);

        void render(const Frame& frame) noexcept(false);

        void onEvent(Event& event);

    private:
        void setUniformData(const VulkanUniformBuffer* uniformBuffer, const Frame& frame) const;

        void bindDescriptorSet(VkDescriptorSet descriptorSet, const VulkanCommandBuffer& commandBuffer) const;

        void drawIndexed(const VulkanCommandBuffer& commandBuffer) const;

        void recompileShaders();

        void compileShaders() const;

        void createTextureSampler();

        void createDescriptorSetLayout();

        void createDescriptorPool();

        void createDescriptorSets();

        void createGraphicsPipelineObjects();

        void destroyGraphicsPipelineObjects() const;
    };
}
