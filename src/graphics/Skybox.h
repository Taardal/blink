#pragma once

#include "system/FileSystem.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanCommandPool.h"
#include "graphics/VulkanUniformBuffer.h"
#include "graphics/VulkanImage.h"
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
    private:
        static constexpr uint32_t FACE_COUNT = 6;
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;

    private:
        SkyboxConfig config;
        VulkanCommandPool* commandPool = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;
        std::vector<VulkanUniformBuffer*> uniformBuffers;
        VkSampler textureSampler = nullptr;
        std::array<std::shared_ptr<VulkanImage>, FACE_COUNT> textures;
        VkImage image = nullptr;
        VkImageView imageView = nullptr;
        VkDeviceMemory deviceMemory = nullptr;
        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;

    public:
        Skybox(const SkyboxConfig& config);

        ~Skybox();

    private:
        void createGraphicsPipeline(std::shared_ptr<VulkanShader> vertexShader, std::shared_ptr<VulkanShader> fragmentShader);

        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image, VkFormat format, VkCommandBuffer commandBuffer, VkImageSubresourceRange subresourceRange) const;

        void createCommandPool();

        void destroyCommandPool() const;

        void createDescriptorPool();

        void destroyDescriptorPool() const;

        void createDescriptorSetLayout();

        void destroyDescriptorSetLayout() const;

        void createDescriptorSets();

        void destroyDescriptorSets() const;

        void createUniformBuffers();

        void destroyUniformBuffers();

        void createTextureSampler();

        void destroyTextureSampler() const;

        void createTextures();

        void destroyTextures();
    };
}
