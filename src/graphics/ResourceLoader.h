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

    struct ResourceLoaderConfig {
        FileSystem* fileSystem = nullptr;
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
    };

    class ResourceLoader {
    private:
        static constexpr uint32_t MAX_MESHES = 100;
        static constexpr uint32_t MAX_TEXTURES_PER_MESH = 16;

    private:
        ResourceLoaderConfig config;
        std::shared_ptr<ImageFile> placeholderTextureImage = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkSampler textureSampler = nullptr;

    public:
        explicit ResourceLoader(const ResourceLoaderConfig& config);

        ~ResourceLoader();

        VkDescriptorSetLayout getDescriptorSetLayout() const;

        std::shared_ptr<Mesh> loadModel(const MeshInfo& meshInfo) const;

    private:
        void createDescriptorPool();

        void createDescriptorSetLayout();

        void createTextureSampler();
    };
}
