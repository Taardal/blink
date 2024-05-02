#pragma once

#include "system/FileSystem.h"
#include "graphics/Mesh.h"
#include "graphics/VulkanCommandPool.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanVertexBuffer.h"
#include "graphics/VulkanIndexBuffer.h"
#include "graphics/VulkanImage.h"

#include <tiny_obj_loader.h>
#include <vector>

namespace Blink {
    struct MeshInfo {
        std::string modelPath;
        std::string textureAtlasPath; // Single texture atlas. Mutually exclusive with textureDirectoryPath.
        std::string texturesDirectoryPath; // Multiple textures. Mutually exclusive with textureAtlasPath.
    };

    struct MeshManagerConfig {
        FileSystem* fileSystem = nullptr;
        VulkanDevice* device = nullptr;
    };

    class MeshManager {
    private:
        static constexpr uint32_t MAX_MESHES = 100;
        static constexpr uint32_t MAX_TEXTURES_PER_MESH = 16;

    private:
        MeshManagerConfig config;
        std::map<std::string, std::shared_ptr<Mesh>> meshCache;
        std::map<std::string, std::shared_ptr<VulkanImage>> textureCache;
        VulkanCommandPool* commandPool = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkSampler textureSampler = nullptr;
        std::shared_ptr<VulkanImage> placeholderTexture = nullptr;

    public:
        explicit MeshManager(const MeshManagerConfig& config);

        ~MeshManager();

        VkDescriptorSetLayout getDescriptorSetLayout() const;

        std::shared_ptr<Mesh> getMesh(const MeshInfo& meshInfo);

    private:
        std::shared_ptr<Mesh> loadMesh(const MeshInfo& meshInfo);

        std::shared_ptr<VulkanImage> createTexture(const std::shared_ptr<ImageFile>& imageFile) const;

        void createCommandPool();

        void createDescriptorPool();

        void createDescriptorSetLayout();

        void createTextureSampler();

        void createPlaceholderTexture();
    };
}
