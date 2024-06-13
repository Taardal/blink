#include "Skybox.h"

namespace Blink {
    const std::vector<glm::vec3> Skybox::VERTICES = {
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
    const std::vector<uint32_t> Skybox::INDICES = {
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

    Skybox::Skybox(const SkyboxConfig& config) : config(config) {
        // Load cube map images from disk (faces)
        BL_LOG_DEBUG("Loading skybox textures");
        std::vector<std::string> spaceBlue = {
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_right.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_left.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_top.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_bot.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_front.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_back.png",
        };
        std::vector<std::string> spaceRed1 = {
            "models/oxar_freighter/Spaceboxes/Red/bkg1_right1.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg1_left2.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg1_top3.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg1_bottom4.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg1_front5.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg1_back6.png",
        };
        std::vector<std::string> spaceRed2 = {
            "models/oxar_freighter/Spaceboxes/Red/bkg2_right1.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg2_left2.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg2_top3.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg2_bottom4.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg2_front5.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg2_back6.png",
        };
        std::vector<std::string> spaceRed3 = {
            "models/oxar_freighter/Spaceboxes/Red/bkg3_right1.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg3_left2.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg3_top3.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg3_bottom4.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg3_front5.png",
            "models/oxar_freighter/Spaceboxes/Red/bkg3_back6.png",
        };
        std::vector<std::string> spaceTeal = {
            "models/oxar_freighter/Spaceboxes/Teal/right.png",
            "models/oxar_freighter/Spaceboxes/Teal/left.png",
            "models/oxar_freighter/Spaceboxes/Teal/top.png",
            "models/oxar_freighter/Spaceboxes/Teal/bot.png",
            "models/oxar_freighter/Spaceboxes/Teal/front.png",
            "models/oxar_freighter/Spaceboxes/Teal/back.png",
        };
        std::vector<std::string> sky = {
            "models/skybox/right.jpg",
            "models/skybox/left.jpg",
            "models/skybox/top.jpg",
            "models/skybox/bottom.jpg",
            "models/skybox/front.jpg",
            "models/skybox/back.jpg",
        };

        std::vector<std::string>& images = sky;
        std::vector<std::shared_ptr<ImageFile>> faces;
        for (int i = 0; i < FACE_COUNT; ++i) {
            faces.push_back(config.fileSystem->readImage(images[i]));
        }

        createCommandPool();
        createImage(faces);
        createSampler();
        createDescriptorPool();
        createDescriptorSetLayout();
        createDescriptorSets();
        createVertexBuffer();
        createIndexBuffer();
    }

    Skybox::~Skybox() {
        delete indexBuffer;
        delete vertexBuffer;
        config.device->destroyDescriptorSetLayout(descriptorSetLayout);
        config.device->destroyDescriptorPool(descriptorPool);
        config.device->destroySampler(sampler);
        delete image;
        delete commandPool;
    }

    void Skybox::createCommandPool() {
        VulkanCommandPoolConfig commandPoolConfig{};
        commandPoolConfig.device = config.device;
        commandPool = new VulkanCommandPool(commandPoolConfig);
    }

    void Skybox::createImage(const std::vector<std::shared_ptr<ImageFile>>& imageFiles) {
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

        image = new VulkanImage(imageConfig);
        image->setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        image->setData(imageFiles);
        image->setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    void Skybox::createSampler() {
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

    void Skybox::createVertexBuffer() {
        VulkanVertexBufferConfig vertexBufferConfig{};
        vertexBufferConfig.device = config.device;
        vertexBufferConfig.commandPool = commandPool;
        vertexBufferConfig.size = sizeof(VERTICES[0]) * VERTICES.size();
        vertexBuffer = new VulkanVertexBuffer(vertexBufferConfig);
        vertexBuffer->setData((void*) VERTICES.data());
    }

    void Skybox::createIndexBuffer() {
        VulkanIndexBufferConfig indexBufferConfig{};
        indexBufferConfig.device = config.device;
        indexBufferConfig.commandPool = commandPool;
        indexBufferConfig.size = sizeof(INDICES[0]) * INDICES.size();
        indexBuffer = new VulkanIndexBuffer(indexBufferConfig);
        indexBuffer->setData((void*) INDICES.data());
    }

    void Skybox::createDescriptorPool() {
        VkDescriptorPoolSize samplerPoolSize{};
        samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerPoolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = 1;
        descriptorPoolCreateInfo.pPoolSizes = &samplerPoolSize;
        descriptorPoolCreateInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createDescriptorPool(&descriptorPoolCreateInfo, &descriptorPool));
    }

    void Skybox::createDescriptorSetLayout() {
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

    void Skybox::createDescriptorSets() {
        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(descriptorSets.size(), descriptorSetLayout);

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = descriptorSets.size();
        descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

        BL_ASSERT_THROW_VK_SUCCESS(config.device->allocateDescriptorSets(&descriptorSetAllocateInfo, descriptorSets.data()));

        for (uint32_t i = 0; i < descriptorSets.size(); i++) {
            VkDescriptorImageInfo descriptorImageInfo{};
            descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptorImageInfo.imageView = image->getImageView();
            descriptorImageInfo.sampler = sampler;

            VkWriteDescriptorSet samplerDescriptorWrite{};
            samplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            samplerDescriptorWrite.dstSet = descriptorSets[i];
            samplerDescriptorWrite.dstBinding = 0;
            samplerDescriptorWrite.dstArrayElement = 0;
            samplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerDescriptorWrite.descriptorCount = 1;
            samplerDescriptorWrite.pImageInfo = &descriptorImageInfo;

            config.device->updateDescriptorSets(1, &samplerDescriptorWrite);
        }
    }
}
