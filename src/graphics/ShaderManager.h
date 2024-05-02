#pragma once

#include "graphics/VulkanDevice.h"
#include "graphics/VulkanShader.h"
#include "system/FileSystem.h"

#include <map>
#include <string>

namespace Blink {
    struct ShaderManagerConfig {
        VulkanDevice* device = nullptr;
        FileSystem* fileSystem = nullptr;
    };

    class ShaderManager {
    private:
        ShaderManagerConfig config;
        std::map<std::string, std::shared_ptr<VulkanShader>> cache;

    public:
        explicit ShaderManager(const ShaderManagerConfig& config);

        std::shared_ptr<VulkanShader> getShader(const std::string& path);

        void reloadShaders();

    private:
        std::shared_ptr<VulkanShader> loadShader(const std::string& path) const;

        void compileShaders() const;
    };
}
