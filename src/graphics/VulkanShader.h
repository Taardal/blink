#pragma once

#include "VulkanDevice.h"
#include <vulkan/vulkan.h>
#include <string>

namespace Blink {
    class VulkanShader {
    private:
        VulkanDevice* vulkanDevice;
        VkShaderModule shaderModule = VK_NULL_HANDLE;

    public:
        explicit VulkanShader(VulkanDevice* vulkanDevice);

        VkShaderModule getModule() const;

        bool initialize(const std::vector<char>& bytes);

        void terminate();
    };
}
