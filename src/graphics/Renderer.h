#pragma once

#include "system/FileSystem.h"
#include "window/Window.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanShader.h"
#include "graphics/VulkanGraphicsPipeline.h"
#include "graphics/VulkanUniformBuffer.h"
#include "graphics/ViewProjection.h"
#include "graphics/MeshManager.h"
#include "graphics/ShaderManager.h"
#include "graphics/SkyboxManager.h"
#include "graphics/Skybox.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct RendererConfig {
        FileSystem* fileSystem = nullptr;
        Window* window = nullptr;
        VulkanApp* vulkanApp = nullptr;
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VulkanDevice* device = nullptr;
        ShaderManager* shaderManager = nullptr;
        MeshManager* meshManager = nullptr;
        SkyboxManager* skyboxManager = nullptr;
    };

    class Renderer {
    private:
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;

    private:
        RendererConfig config;
        VulkanSwapChain* swapChain = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        std::vector<VulkanCommandBuffer> commandBuffers;
        std::vector<VulkanUniformBuffer*> viewProjectionUniformBuffers;
        VkDescriptorPool viewProjectionDescriptorPool = nullptr;
        VkDescriptorSetLayout viewProjectionDescriptorSetLayout = nullptr;
        std::vector<VkDescriptorSet> viewProjectionDescriptorSets;
        VulkanGraphicsPipeline* meshGraphicsPipeline = nullptr;
        VulkanGraphicsPipeline* skyboxGraphicsPipeline = nullptr;
        VulkanCommandBuffer currentCommandBuffer;
        uint32_t currentFrame = 0;

    public:
        explicit Renderer(const RendererConfig& config);

        ~Renderer();

        void waitUntilIdle() const;

        void onEvent(Event& event);

        bool beginFrame();

        void setViewProjection(const ViewProjection& viewProjection) const;

        void renderSkybox(const std::shared_ptr<Skybox>& skybox) const;

        void renderMesh(const std::shared_ptr<Mesh>& mesh) const;

        void endFrame();

    private:
        void reloadShaders();

        void createCommandObjects();

        void destroyCommandObjects() const;

        void createSwapChain();

        void destroySwapChain() const;

        void createUniformBuffers();

        void destroyUniformBuffers();

        void createDescriptorObjects();

        void destroyDescriptorObjects() const;

        void createGraphicsPipelines();

        void destroyGraphicsPipelines() const;
    };
}
