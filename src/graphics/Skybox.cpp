#include "Skybox.h"

#include "UniformBufferData.h"

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
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSetLayout();
        createDescriptorSets();
        createGraphicsPipeline();
    }

    Skybox::~Skybox() {
        delete graphicsPipeline;

        config.device->destroyDescriptorSetLayout(descriptorSetLayout);
        config.device->destroyDescriptorPool(descriptorPool);

        for (int i = 0; i < uniformBuffers.size(); ++i) {
            delete uniformBuffers[i];
        }
        uniformBuffers.clear();

        delete indexBuffer;
        delete vertexBuffer;

        config.device->destroySampler(sampler);
        delete image;
        delete commandPool;
    }

    void Skybox::render(const VulkanCommandBuffer& commandBuffer, uint32_t currentFrame) const {
        graphicsPipeline->bind(commandBuffer);
        vertexBuffer->bind(commandBuffer);
        indexBuffer->bind(commandBuffer);

        VkDescriptorSet descriptorSet = descriptorSets[currentFrame];

        constexpr uint32_t firstSet = 0;
        constexpr uint32_t descriptorSetCount = 1;
        constexpr uint32_t dynamicOffsetCount = 0;
        constexpr uint32_t* dynamicOffsets = nullptr;
        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphicsPipeline->getLayout(),
            firstSet,
            descriptorSetCount,
            &descriptorSet,
            dynamicOffsetCount,
            dynamicOffsets
        );

        constexpr uint32_t instanceCount = 1;
        constexpr uint32_t firstIndex = 0;
        constexpr uint32_t vertexOffset = 0;
        constexpr uint32_t firstInstance = 0;
        vkCmdDrawIndexed(
            commandBuffer,
            (uint32_t) INDICES.size(),
            instanceCount,
            firstIndex,
            vertexOffset,
            firstInstance
        );
    }

    void Skybox::setUniformData(const ViewProjection& viewProjection, uint32_t currentFrame) const {
        VulkanUniformBuffer* uniformBuffer = uniformBuffers[currentFrame];

        UniformBufferData uniformBufferData{};
        uniformBufferData.view = viewProjection.view;
        uniformBufferData.projection = viewProjection.projection;
        uniformBufferData.projection[1][1] *= -1;

        uniformBuffer->setData(&uniformBufferData);
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

    void Skybox::createUniformBuffers() {
        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for (uint32_t i = 0; i < uniformBuffers.size(); i++) {
            VulkanUniformBufferConfig uniformBufferConfig{};
            uniformBufferConfig.device = config.device;
            uniformBufferConfig.commandPool = commandPool;
            uniformBufferConfig.size = sizeof(UniformBufferData);
            uniformBuffers[i] = new VulkanUniformBuffer(uniformBufferConfig);
        }
    }

    void Skybox::createDescriptorPool() {
        VkDescriptorPoolSize uniformBufferPoolSize{};
        uniformBufferPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferPoolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

        VkDescriptorPoolSize textureSamplerPoolSize{};
        textureSamplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureSamplerPoolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

        std::array<VkDescriptorPoolSize, 2> poolSizes = {
            uniformBufferPoolSize,
            textureSamplerPoolSize,
        };

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = poolSizes.size();
        descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
        descriptorPoolCreateInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createDescriptorPool(&descriptorPoolCreateInfo, &descriptorPool));
    }

    void Skybox::createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uniformBufferLayoutBinding{};
        uniformBufferLayoutBinding.binding = 0;
        uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferLayoutBinding.descriptorCount = 1;
        uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding textureSamplerLayoutBinding{};
        textureSamplerLayoutBinding.binding = 1;
        textureSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureSamplerLayoutBinding.descriptorCount = 1;
        textureSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> descriptorSetLayoutBindings = {
            uniformBufferLayoutBinding,
            textureSamplerLayoutBinding,
        };

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = descriptorSetLayoutBindings.size();
        descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

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
            VkDescriptorBufferInfo uniformBufferDescriptorBufferInfo{};
            uniformBufferDescriptorBufferInfo.buffer = *uniformBuffers[i];
            uniformBufferDescriptorBufferInfo.offset = 0;
            uniformBufferDescriptorBufferInfo.range = sizeof(UniformBufferData);

            VkWriteDescriptorSet uniformBufferDescriptorWrite{};
            uniformBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            uniformBufferDescriptorWrite.dstSet = descriptorSets[i];
            uniformBufferDescriptorWrite.dstBinding = 0;
            uniformBufferDescriptorWrite.dstArrayElement = 0;
            uniformBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uniformBufferDescriptorWrite.descriptorCount = 1;
            uniformBufferDescriptorWrite.pBufferInfo = &uniformBufferDescriptorBufferInfo;

            VkDescriptorImageInfo descriptorImageInfo{};
            descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptorImageInfo.imageView = image->getImageView();
            descriptorImageInfo.sampler = sampler;

            VkWriteDescriptorSet textureSamplerDescriptorWrite{};
            textureSamplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            textureSamplerDescriptorWrite.dstSet = descriptorSets[i];
            textureSamplerDescriptorWrite.dstBinding = 1;
            textureSamplerDescriptorWrite.dstArrayElement = 0;
            textureSamplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            textureSamplerDescriptorWrite.descriptorCount = 1;
            textureSamplerDescriptorWrite.pImageInfo = &descriptorImageInfo;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites = {
                uniformBufferDescriptorWrite,
                textureSamplerDescriptorWrite,
            };

            config.device->updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data());
        }
    }

    void Skybox::createGraphicsPipeline() {
        std::shared_ptr<VulkanShader> vertexShader = config.shaderManager->getShader("shaders/skybox.vert.spv");
        std::shared_ptr<VulkanShader> fragmentShader = config.shaderManager->getShader("shaders/skybox.frag.spv");

        VkVertexInputBindingDescription vertexInputBindingDescription{};
        vertexInputBindingDescription.binding = 0;
        vertexInputBindingDescription.stride = sizeof(glm::vec3);
        vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkVertexInputAttributeDescription vertexInputAttributeDescription{};
        vertexInputAttributeDescription.binding = 0;
        vertexInputAttributeDescription.location = 0;
        vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexInputAttributeDescription.offset = 0;

        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions{
            vertexInputAttributeDescription,
        };

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {
            descriptorSetLayout,
        };

        VulkanGraphicsPipelineConfig graphicsPipelineConfig{};
        graphicsPipelineConfig.device = config.device;
        graphicsPipelineConfig.renderPass = config.swapChain->getRenderPass();
        graphicsPipelineConfig.vertexShader = vertexShader;
        graphicsPipelineConfig.fragmentShader = fragmentShader;
        graphicsPipelineConfig.vertexBindingDescription = &vertexInputBindingDescription;
        graphicsPipelineConfig.vertexAttributeDescriptions = &vertexInputAttributeDescriptions;
        graphicsPipelineConfig.descriptorSetLayouts = &descriptorSetLayouts;
        graphicsPipelineConfig.depthTestEnabled = false;

        graphicsPipeline = new VulkanGraphicsPipeline(graphicsPipelineConfig);
    }
}
