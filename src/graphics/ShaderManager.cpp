#include "ShaderManager.h"

namespace Blink {
    ShaderManager::ShaderManager(const ShaderManagerConfig& config) : config(config) {
    }

    std::shared_ptr<VulkanShader> ShaderManager::getShader(const std::string& path) {
        const auto iterator = cache.find(path);
        if (iterator != cache.end()) {
            return iterator->second;
        }
        std::shared_ptr<VulkanShader> shader = loadShader(path);
        cache[path] = shader;
        return shader;
    }

    void ShaderManager::reloadShaders() {
        compileShaders();
        for (const auto& [path, _] : cache) {
            cache[path] = loadShader(path);
        }
    }

    std::shared_ptr<VulkanShader> ShaderManager::loadShader(const std::string& path) const {
        VulkanShaderConfig shaderConfig{};
        shaderConfig.bytes = config.fileSystem->readBytes(path);
        shaderConfig.device = config.device;
        return std::make_shared<VulkanShader>(shaderConfig);
    }

    void ShaderManager::compileShaders() const {
        std::stringstream ss;
        ss << "cmake";
        ss << " -D SHADERS_SOURCE_DIR=" << CMAKE_SHADERS_SOURCE_DIR;
        ss << " -D SHADERS_OUTPUT_DIR=" << CMAKE_SHADERS_OUTPUT_DIR;
        ss << " -P " << CMAKE_SCRIPTS_DIR << "/compile_shaders.cmake";
        std::string command = ss.str();
        std::system(command.c_str());
    }
}
