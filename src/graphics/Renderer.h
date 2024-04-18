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
#include "graphics/Model.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Blink {
    struct Frame {
        //glm::mat4* model;
        glm::mat4* view;
        glm::mat4* projection;
        //std::vector<Vertex>* vertices;
        //std::vector<uint16_t>* indices;
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
        std::vector<VulkanUniformBuffer*> uniformBuffers;
        VkSampler textureSampler;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;
        VulkanShader* vertexShader = nullptr;
        VulkanShader* fragmentShader = nullptr;
        VulkanGraphicsPipeline* graphicsPipeline = nullptr;
        uint32_t currentFrame = 0;

    public:
        explicit Renderer(const RendererConfig& config);

        ~Renderer();

        void waitUntilIdle() const;

        void onEvent(Event& event);

        bool beginFrame(const Frame& frame) const;

        void render(const Frame& frame, const Model& model) const;

        void endFrame();

        Model createModel() const;

        void destroyModel(const Model& model) const;

    private:
        void setUniformData(const VulkanUniformBuffer* uniformBuffer, const Frame& frame) const;

        void reloadShaders();

        void compileShaders() const;

        void createTextureSampler();

        void createDescriptorSetLayout();

        void createDescriptorPool();

        void createDescriptorSets();

        void createGraphicsPipelineObjects();

        void destroyGraphicsPipelineObjects() const;
    };
}
