#pragma once

#include "graphics/MeshManager.h"
#include "graphics/ViewProjection.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanShader.h"
#include "graphics/VulkanGraphicsPipeline.h"
#include "graphics/VulkanUniformBuffer.h"
#include "system/FileSystem.h"
#include "window/Window.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct RendererConfig {
        VulkanApp* vulkanApp = nullptr;
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VulkanDevice* device = nullptr;
        FileSystem* fileSystem = nullptr;
        Window* window = nullptr;
        MeshManager* meshManager = nullptr;
    };

    class Renderer {
    private:
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;

    private:
        RendererConfig config;
        VulkanSwapChain* swapChain = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        std::vector<VulkanCommandBuffer> commandBuffers;
        std::vector<VulkanUniformBuffer*> uniformBuffers;
        VkDescriptorPool descriptorPool = nullptr;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;
        VulkanGraphicsPipeline* graphicsPipeline = nullptr;
        uint32_t currentFrame = 0;
        VulkanCommandBuffer currentCommandBuffer = nullptr;

    public:
        explicit Renderer(const RendererConfig& config);

        ~Renderer();

        void waitUntilIdle() const;

        void onEvent(Event& event);

        bool beginFrame();

        void renderMesh(const std::shared_ptr<Mesh>& mesh, const ViewProjection& viewProjection) const;

        void endFrame();

    private:
        void setUniformData(const ViewProjection& viewProjection) const;

        void setPushConstantData(const std::shared_ptr<Mesh>& mesh) const;

        void bindMesh(const std::shared_ptr<Mesh>& mesh) const;

        void drawMeshIndexed(const std::shared_ptr<Mesh>& mesh) const;

        void reloadShaders();

        void compileShaders() const;

        void createCommandObjects();

        void destroyCommandObjects() const;

        void createSwapChain();

        void destroySwapChain() const;

        void createUniformBuffers();

        void destroyUniformBuffers();

        void createDescriptorObjects();

        void destroyDescriptorObjects() const;

        void createGraphicsPipeline();

        void destroyGraphicsPipeline() const;
    };
}
