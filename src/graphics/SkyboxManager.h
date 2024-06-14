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
#include "graphics/Skybox.h"

namespace Blink {
    struct SkyboxManagerConfig {
        FileSystem* fileSystem = nullptr;
        VulkanDevice* device = nullptr;
    };

    class SkyboxManager {
    private:
        static constexpr uint32_t MAX_SKYBOXES = 10;
        static const std::vector<SkyboxVertex> SKYBOX_VERTICES;
        static const std::vector<uint32_t> SKYBOX_INDICES;

    private:
        SkyboxManagerConfig config;
        std::map<std::string, std::shared_ptr<Skybox>> cache;
        VulkanCommandPool* commandPool = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkSampler sampler = nullptr;

    public:
        explicit SkyboxManager(const SkyboxManagerConfig& config);

        ~SkyboxManager();

        VkDescriptorSetLayout getDescriptorSetLayout() const;

        void clear();

        std::shared_ptr<Skybox> getSkybox(const std::vector<std::string>& paths);

    private:
        std::shared_ptr<Skybox> loadSkybox(const std::vector<std::string>& paths) const;

        void createCommandPool();

        void destroyCommandPool() const;

        void createDescriptorPool();

        void destroyDescriptorPool() const;

        void createDescriptorSetLayout();

        void destroyDescriptorSetLayout() const;

        void createSampler();

        void destroySampler() const;
    };
}
