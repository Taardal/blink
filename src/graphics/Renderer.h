#pragma once

#include "system/FileSystem.h"
#include "window/Window.h"
#include "graphics/ResourceLoader.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanShader.h"
#include "graphics/VulkanGraphicsPipeline.h"
#include "graphics/VulkanUniformBuffer.h"
#include "graphics/ViewProjection.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct RendererConfig {
        VulkanApp* vulkanApp = nullptr;
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        FileSystem* fileSystem = nullptr;
        Window* window = nullptr;
        ResourceLoader* resourceLoader = nullptr;
    };

    class Renderer {
    private:
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;
        static constexpr uint32_t MAX_MESHES = 100;
        static constexpr uint32_t MAX_DESCRIPTORS_PER_STAGE = 16;

    private:
        RendererConfig config;
        VulkanSwapChain* swapChain = nullptr;
        std::vector<VulkanCommandBuffer> commandBuffers;
        std::vector<VulkanUniformBuffer*> uniformBuffers;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;
        VulkanShader* vertexShader = nullptr;
        VulkanShader* fragmentShader = nullptr;
        VulkanGraphicsPipeline* graphicsPipeline = nullptr;
        uint32_t currentFrame = 0;
        VulkanCommandBuffer currentCommandBuffer = nullptr;

    public:
        explicit Renderer(const RendererConfig& config);

        ~Renderer();

        void waitUntilIdle() const;

        void onEvent(Event& event);

        bool beginFrame();

        void renderMesh(std::shared_ptr<Mesh> mesh, const ViewProjection& viewProjection) const;

        void endFrame();

    private:
        void setUniformData(const ViewProjection& viewProjection) const;

        void setPushConstantData(std::shared_ptr<Mesh> mesh) const;

        void bindMesh(std::shared_ptr<Mesh> mesh) const;

        void drawMeshIndexed(std::shared_ptr<Mesh> mesh) const;

        void reloadShaders();

        void compileShaders() const;

        void createCommandObjects();

        void destroyCommandObjects() const;

        void createUniformBuffers();

        void destroyUniformBuffers();

        void createDescriptorObjects();

        void destroyDescriptorObjects() const;

        void createGraphicsPipelineObjects();

        void destroyGraphicsPipelineObjects() const;
    };
}
