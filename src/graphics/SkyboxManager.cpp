#include "SkyboxManager.h"

namespace Blink {
    const std::vector<glm::vec3> SkyboxManager::SKYBOX_VERTICES = {
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
    const std::vector<uint32_t> SkyboxManager::SKYBOX_INDICES = {
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

    SkyboxManager::SkyboxManager(const SkyboxManagerConfig& config) : config(config) {
        createCommandPool();
        createDescriptorPool();
        createDescriptorSetLayout();
        createSampler();
    }

    SkyboxManager::~SkyboxManager() {
        destroySampler();
        destroyDescriptorSetLayout();
        destroyDescriptorPool();
        destroyCommandPool();
    }

    VkDescriptorSetLayout SkyboxManager::getDescriptorSetLayout() const {
        return descriptorSetLayout;
    }

    void SkyboxManager::resetDescriptors() {
        destroyDescriptorPool();
        createDescriptorPool();
    }

    std::shared_ptr<Skybox> SkyboxManager::getSkybox(const std::vector<std::string>& paths) {
        BL_ASSERT_THROW(paths.size() == FACE_COUNT);

        const std::string& key = paths[0];
        BL_ASSERT_THROW(!key.empty());

        const auto iterator = cache.find(key);
        if (iterator != cache.end()) {
            return iterator->second;
        }
        std::shared_ptr<Skybox> skybox = loadSkybox(paths);
        cache[key] = skybox;
        return skybox;
    }

    std::shared_ptr<Skybox> SkyboxManager::loadSkybox(const std::vector<std::string>& paths) const {
        std::vector<std::shared_ptr<ImageFile>> imageFiles;
        for (int i = 0; i < FACE_COUNT; ++i) {
            imageFiles.push_back(config.fileSystem->readImage(paths[i]));
        }
        BL_ASSERT_THROW(imageFiles.size() == paths.size());

        VulkanImageConfig imageConfig{};
        imageConfig.device = config.device;
        imageConfig.commandPool = commandPool;
        imageConfig.width = imageFiles[0]->width;
        imageConfig.height = imageFiles[0]->height;
        imageConfig.layerCount = imageFiles.size();
        imageConfig.imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
        imageConfig.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        imageConfig.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageConfig.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        auto image = std::make_shared<VulkanImage>(imageConfig);
        image->setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        image->setData(imageFiles);
        image->setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        VkDescriptorSet descriptorSet;

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->allocateDescriptorSets(&descriptorSetAllocateInfo, &descriptorSet));

        VkDescriptorImageInfo descriptorImageInfo{};
        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptorImageInfo.imageView = image->getImageView();
        descriptorImageInfo.sampler = sampler;

        VkWriteDescriptorSet samplerDescriptorWrite{};
        samplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        samplerDescriptorWrite.dstSet = descriptorSet;
        samplerDescriptorWrite.dstBinding = 0;
        samplerDescriptorWrite.dstArrayElement = 0;
        samplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerDescriptorWrite.descriptorCount = 1;
        samplerDescriptorWrite.pImageInfo = &descriptorImageInfo;

        config.device->updateDescriptorSets(1, &samplerDescriptorWrite);

        VulkanVertexBufferConfig vertexBufferConfig{};
        vertexBufferConfig.device = config.device;
        vertexBufferConfig.commandPool = commandPool;
        vertexBufferConfig.size = sizeof(SKYBOX_VERTICES[0]) * SKYBOX_VERTICES.size();
        auto vertexBuffer = std::make_shared<VulkanVertexBuffer>(vertexBufferConfig);
        vertexBuffer->setData((void*) SKYBOX_VERTICES.data());

        VulkanIndexBufferConfig indexBufferConfig{};
        indexBufferConfig.device = config.device;
        indexBufferConfig.commandPool = commandPool;
        indexBufferConfig.size = sizeof(SKYBOX_INDICES[0]) * SKYBOX_INDICES.size();
        auto indexBuffer = std::make_shared<VulkanIndexBuffer>(indexBufferConfig);
        indexBuffer->setData((void*) SKYBOX_INDICES.data());

        auto skybox = std::make_shared<Skybox>();
        skybox->image = image;
        skybox->descriptorSet = descriptorSet;
        skybox->vertexBuffer = vertexBuffer;
        skybox->indexBuffer = indexBuffer;
        skybox->vertices = SKYBOX_VERTICES;
        skybox->indices = SKYBOX_INDICES;
        return skybox;
    }

    void SkyboxManager::createCommandPool() {
        VulkanCommandPoolConfig commandPoolConfig{};
        commandPoolConfig.device = config.device;
        commandPool = new VulkanCommandPool(commandPoolConfig);
    }

    void SkyboxManager::destroyCommandPool() const {
        delete commandPool;
    }

    void SkyboxManager::createDescriptorPool() {
        VkDescriptorPoolSize samplerPoolSize{};
        samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerPoolSize.descriptorCount = MAX_SKYBOXES;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = 1;
        descriptorPoolCreateInfo.pPoolSizes = &samplerPoolSize;
        descriptorPoolCreateInfo.maxSets = MAX_SKYBOXES;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createDescriptorPool(&descriptorPoolCreateInfo, &descriptorPool));
    }

    void SkyboxManager::destroyDescriptorPool() const {
        config.device->destroyDescriptorPool(descriptorPool);
    }

    void SkyboxManager::createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 0;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = 1;
        descriptorSetLayoutCreateInfo.pBindings = &samplerLayoutBinding;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, &descriptorSetLayout));
    }

    void SkyboxManager::destroyDescriptorSetLayout() const {
        config.device->destroyDescriptorSetLayout(descriptorSetLayout);
    }

    void SkyboxManager::createSampler() {
        VulkanPhysicalDevice* physicalDevice = config.device->getPhysicalDevice();

        VkSamplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeU;
        samplerCreateInfo.addressModeW = samplerCreateInfo.addressModeU;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 1.0f;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        samplerCreateInfo.maxAnisotropy = physicalDevice->getProperties().limits.maxSamplerAnisotropy;
        samplerCreateInfo.anisotropyEnable = VK_TRUE;

        config.device->createSampler(&samplerCreateInfo, &sampler);
    }

    void SkyboxManager::destroySampler() const {
        config.device->destroySampler(sampler);
    }
}
