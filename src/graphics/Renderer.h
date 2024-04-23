#pragma once

#include "system/FileSystem.h"
#include "window/Window.h"
#include "graphics/ResourceLoader.h"
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
#include "graphics/Mesh.h"
#include "graphics/ViewProjection.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Blink {
    struct RendererConfig {
        std::string applicationName;
        std::string engineName;
        FileSystem* fileSystem = nullptr;
        Window* window = nullptr;
        ResourceLoader* resourceLoader = nullptr;
    };

    class Renderer {
    private:
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;
        static constexpr uint32_t MAX_MESHES = 100;

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
        VkDescriptorSetLayout perFrameDescriptorSetLayout = nullptr;
        VkDescriptorSetLayout perMeshDescriptorSetLayout = nullptr;
        VkDescriptorPool perFrameDescriptorPool = nullptr;
        VkDescriptorPool perMeshDescriptorPool = nullptr;
        std::vector<VkDescriptorSet> perFrameDescriptorSets;
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

        void renderMesh(const Mesh& mesh, const ViewProjection& viewProjection) const;

        void endFrame();

        Mesh createMesh() const;

        void destroyMesh(const Mesh& mesh) const;

    private:
        void setUniformData(const ViewProjection& viewProjection) const;

        void setPushConstantData(const Mesh& mesh) const;

        void bindMesh(const Mesh& mesh) const;

        void drawMeshIndexed(const Mesh& mesh) const;

        void reloadShaders();

        void compileShaders() const;

        void createTextureSampler();

        void createDescriptorSetLayouts();

        void createDescriptorPools();

        void createDescriptorSets();

        void createGraphicsPipelineObjects();

        void destroyGraphicsPipelineObjects() const;
    };
}
