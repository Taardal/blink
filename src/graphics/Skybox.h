#pragma once

#include "system/FileSystem.h"
#include "graphics/ViewProjection.h"
#include "graphics/VulkanIndexBuffer.h"
#include "graphics/VulkanVertexBuffer.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanCommandPool.h"
#include "graphics/VulkanUniformBuffer.h"
#include "graphics/VulkanImage.h"
#include "graphics/VulkanGraphicsPipeline.h"
#include "graphics/VulkanShader.h"
#include "graphics/ShaderManager.h"

namespace Blink {
    struct SkyboxConfig {
        FileSystem* fileSystem = nullptr;
        VulkanDevice* device = nullptr;
        VulkanSwapChain* swapChain = nullptr;
        ShaderManager* shaderManager = nullptr;
    };

    class Skybox {
    public:
        static constexpr uint32_t FACE_COUNT = 6;
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;
        static const std::vector<glm::vec3> VERTICES;
        static const std::vector<uint32_t> INDICES;

    public:
        SkyboxConfig config;
        VulkanCommandPool* commandPool = nullptr;
        VulkanImage* image = nullptr;
        VkSampler sampler = nullptr;
        VulkanVertexBuffer* vertexBuffer = nullptr;
        VulkanIndexBuffer* indexBuffer = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;

    public:
        explicit Skybox(const SkyboxConfig& config);

        ~Skybox();

    private:
        void createCommandPool();

        void createImage(const std::vector<std::shared_ptr<ImageFile>>& imageFiles);

        void createSampler();

        void createDescriptorPool();

        void createDescriptorSetLayout();

        void createDescriptorSets();

        void createVertexBuffer();

        void createIndexBuffer();
    };
}
