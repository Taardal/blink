#pragma once

#include "system/FileSystem.h"
#include "system/ObjFile.h"
#include "graphics/Mesh.h"
#include "graphics/VulkanCommandPool.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanVertexBuffer.h"
#include "graphics/VulkanIndexBuffer.h"
#include "graphics/VulkanImage.h"

#include <vector>

namespace Blink {
    struct MeshInfo {
        std::string modelPath;
        std::string textureAtlasPath; // Single texture atlas file. Mutually exclusive with textureDirectoryPath.
        std::string texturesDirectoryPath; // Multiple texture files. Mutually exclusive with textureAtlasPath.
    };

    struct MeshManagerConfig {
        FileSystem* fileSystem = nullptr;
        VulkanDevice* device = nullptr;
    };

    class MeshManager {
    private:
        static constexpr uint32_t MAX_MESHES = 1000;
        static constexpr uint32_t MAX_TEXTURES_PER_MESH = 16; // `uniform sampler2D textureSamplers[16];` @ fragment shader
        static constexpr uint32_t MAX_TEXTURES = MAX_MESHES * MAX_TEXTURES_PER_MESH;

    private:
        MeshManagerConfig config;
        std::map<std::string, std::shared_ptr<ObjFile>> objCache;
        std::map<std::string, std::shared_ptr<ImageFile>> imageCache;
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

        void resetDescriptors();

    private:
        std::shared_ptr<ObjFile> getObjFile(const std::string& path);

        std::shared_ptr<ImageFile> getImageFile(const std::string& path);

        std::shared_ptr<VulkanImage> createTexture(const std::shared_ptr<ImageFile>& imageFile) const;

        void createCommandPool();

        void destroyCommandPool() const;

        void createDescriptorPool();

        void destroyDescriptorPool() const;

        void createDescriptorSetLayout();

        void destroyDescriptorSetLayout() const;

        void createTextureSampler();

        void destroyTextureSampler();

        void createPlaceholderTexture();
    };
}
