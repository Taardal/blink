#pragma once

#include "system/FileSystem.h"
#include "graphics/Image.h"
#include "graphics/Model.h"
#include "graphics/VulkanCommandPool.h"
#include "graphics/VulkanDevice.h"

#include <vector>

namespace Blink {
    struct ResourceLoaderConfig {
        FileSystem* fileSystem = nullptr;
        //VulkanDevice* device = nullptr;
        //VulkanCommandPool* commandPool = nullptr;
        //VkDescriptorPool descriptorPool = nullptr;
    };

    class ResourceLoader {
    private:
        static const std::string MODELS_PATH;
        static const std::string SHADERS_PATH;
        static const std::string TEXTURES_PATH;

    private:
        ResourceLoaderConfig config;

    public:
        explicit ResourceLoader(const ResourceLoaderConfig& config);

        std::shared_ptr<Image> loadTexture(const std::string& name) const;

        std::shared_ptr<Model> loadModel(const std::string& name) const;
    };
}
