#pragma once

#include "ViewProjection.h"
#include "VulkanIndexBuffer.h"
#include "VulkanVertexBuffer.h"
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
        VulkanVertexBuffer* vertexBuffer;
        VulkanIndexBuffer* indexBuffer;

        std::vector<glm::vec3> vertices = {
            // Positive X (Right)
            {1, 1, 1},
            {1, 1, -1},
            {1, -1, 1},
            {1, -1, -1},
            // Negative X (Left)
            {-1, 1, 1},
            {-1, 1, -1},
            {-1, -1, 1},
            {-1, -1, -1},
        };

        const std::vector<uint32_t> indices = {
            // Right (Positive X)
            0, 1, 3, 0, 3, 2,
            // Left (Negative X)
            4, 6, 7, 4, 7, 5,
            // Up (Positive Y)
            0, 4, 5, 0, 5, 1,
            // Down (Negative Y)
            2, 3, 7, 2, 7, 6,
            // Front (Positive Z)
            0, 2, 6, 0, 6, 4,
            // Back (Negative Z)
            1, 5, 7, 1, 7, 3
        };

        std::vector<glm::vec3> vertices1 = {
            // positions
            {-1.0f,  1.0f, -1.0f},
            {-1.0f, -1.0f, -1.0f},
             {1.0f, -1.0f, -1.0f},
             {1.0f, -1.0f, -1.0f},
             {1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},

            {-1.0f, -1.0f,  1.0f},
            {-1.0f, -1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f,  1.0f},
            {-1.0f, -1.0f,  1.0f},

             {1.0f, -1.0f, -1.0f},
             {1.0f, -1.0f,  1.0f},
             {1.0f,  1.0f,  1.0f},
             {1.0f,  1.0f,  1.0f},
             {1.0f,  1.0f, -1.0f},
             {1.0f, -1.0f, -1.0f},

            {-1.0f, -1.0f,  1.0f},
            {-1.0f,  1.0f,  1.0f},
             {1.0f,  1.0f,  1.0f},
             {1.0f,  1.0f,  1.0f},
             {1.0f, -1.0f,  1.0f},
            {-1.0f, -1.0f,  1.0f},

            {-1.0f,  1.0f, -1.0f},
             {1.0f,  1.0f, -1.0f},
             {1.0f,  1.0f,  1.0f},
             {1.0f,  1.0f,  1.0f},
            {-1.0f,  1.0f,  1.0f},
            {-1.0f,  1.0f, -1.0f},

            {-1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f,  1.0f},
             {1.0f, -1.0f, -1.0f},
             {1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f,  1.0f},
             {1.0f, -1.0f,  1.0f}
        };

    public:
        explicit Skybox(const SkyboxConfig& config);

        ~Skybox();

        void render(const VulkanCommandBuffer& commandBuffer, uint32_t currentFrame);

        void setUniformData(const ViewProjection& viewProjection, uint32_t currentFrame) const;

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
