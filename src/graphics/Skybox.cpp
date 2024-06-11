#include "Skybox.h"

#include "UniformBufferData.h"

namespace Blink {
    Skybox::Skybox(const SkyboxConfig& config) : config(config) {
        createCommandPool();

        // Load cube map images from disk (faces)
        BL_LOG_DEBUG("Loading skybox textures");
        std::vector<std::string> images = {
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_back.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_bot.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_front.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_left.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_right.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_top.png"
        };
        std::vector<std::shared_ptr<ImageFile>> faces;
        for (int i = 0; i < images.size(); ++i) {
            faces.push_back(config.fileSystem->readImage(images[i]));
        }
        BL_ASSERT(faces.size() == images.size());

        std::shared_ptr<ImageFile> face = faces[0];
        for (int i = 0; i < faces.size(); ++i) {
            BL_ASSERT(faces[i]->pixels != nullptr);
            BL_ASSERT(faces[i]->width == face->width);
            BL_ASSERT(faces[i]->height == face->height);
        }

        VulkanCommandBuffer commandBuffer;
        BL_ASSERT_THROW_VK_SUCCESS(commandPool->allocateCommandBuffer(&commandBuffer));
        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.begin());

        const VkDeviceSize imageSize = face->size * FACE_COUNT;
        const VkDeviceSize layerSize = face->size;
        auto format = VK_FORMAT_R8G8B8A8_SRGB;

        // Create Vulkan image with 6 layers, one layer per face
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width = face->width;
        imageCreateInfo.extent.height = face->height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.arrayLayers = FACE_COUNT;
        imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        imageCreateInfo.format = format;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.mipLevels = 1;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createImage(&imageCreateInfo, &image));

        VkMemoryRequirements memoryRequirements = config.device->getImageMemoryRequirements(image);

        VulkanPhysicalDevice* physicalDevice = config.device->getPhysicalDevice();
        uint32_t memoryTypeIndex = physicalDevice->getMemoryTypeIndex(memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->allocateMemory(&memoryAllocateInfo, &deviceMemory));
        BL_ASSERT_THROW_VK_SUCCESS(config.device->bindImageMemory(image, deviceMemory));

        VkImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.layerCount = FACE_COUNT;

        VkImageLayout oldLayout = imageCreateInfo.initialLayout;
        VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image, format, commandBuffer, subresourceRange);

        VulkanBufferConfig stagingBufferConfig{};
        stagingBufferConfig.device = config.device;
        stagingBufferConfig.commandPool = commandPool;
        stagingBufferConfig.size = layerSize;
        stagingBufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        std::vector<VulkanBuffer*> stagingBuffers;

        std::vector<VkBufferImageCopy> bufferCopyRegions;
        for (uint32_t i = 0; i < FACE_COUNT; i++) {
            auto face = faces[i];

            auto stagingBuffer = new VulkanBuffer(stagingBufferConfig);
            stagingBuffer->setData(face->pixels);
            stagingBuffers.push_back(stagingBuffer);

            VkBufferImageCopy bufferCopyRegion = {};
            bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            bufferCopyRegion.imageSubresource.baseArrayLayer = i;
            bufferCopyRegion.imageSubresource.layerCount = 1;
            bufferCopyRegion.imageExtent.width = faces[i]->width;
            bufferCopyRegion.imageExtent.height = faces[i]->height;
            bufferCopyRegion.imageExtent.depth = 1;
            bufferCopyRegion.bufferOffset = 0;
            bufferCopyRegions.push_back(bufferCopyRegion);

            vkCmdCopyBufferToImage(
                commandBuffer,
                *stagingBuffer,
                image,
                newLayout,
                1,
                &bufferCopyRegion
            );
        }

        transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, image, format, commandBuffer, subresourceRange);

        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.end());

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffer.vk_ptr();

        BL_ASSERT_THROW_VK_SUCCESS(config.device->submitToGraphicsQueue(&submitInfo));
        BL_ASSERT_THROW_VK_SUCCESS(config.device->waitUntilGraphicsQueueIsIdle());

        commandPool->freeCommandBuffer(commandBuffer.vk_ptr());

        for (auto sb : stagingBuffers) {
            delete sb;
        }

        // Create Vulkan image view with cube type
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        // Cube map view type
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        // 6 array layers (faces)
        imageViewCreateInfo.subresourceRange.layerCount = FACE_COUNT;
        // Set number of mip levels
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.image = image;

        config.device->createImageView(&imageViewCreateInfo, &imageView);

        // Create descriptors --> uniform buffer + sampler (cube)
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
        samplerCreateInfo.maxAnisotropy = config.device->getPhysicalDevice()->getProperties().limits.maxSamplerAnisotropy;
        samplerCreateInfo.anisotropyEnable = VK_TRUE;

        config.device->createSampler(&samplerCreateInfo, &textureSampler);

        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for (uint32_t i = 0; i < uniformBuffers.size(); i++) {
            VulkanUniformBufferConfig uniformBufferConfig{};
            uniformBufferConfig.device = config.device;
            uniformBufferConfig.commandPool = commandPool;
            uniformBufferConfig.size = sizeof(UniformBufferData);
            uniformBuffers[i] = new VulkanUniformBuffer(uniformBufferConfig);
        }

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
            descriptorImageInfo.imageView = imageView;
            descriptorImageInfo.sampler = textureSampler;

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

        // Create graphics pipeline with skybox shaders
        std::shared_ptr<VulkanShader> vertexShader = config.shaderManager->getShader("shaders/skybox.vert.spv");
        std::shared_ptr<VulkanShader> fragmentShader = config.shaderManager->getShader("shaders/skybox.frag.spv");
        createGraphicsPipeline(vertexShader, fragmentShader);

        // Vertex and index buffers
        VulkanVertexBufferConfig vertexBufferConfig{};
        vertexBufferConfig.device = config.device;
        vertexBufferConfig.commandPool = commandPool;
        vertexBufferConfig.size = sizeof(vertices[0]) * vertices.size();
        vertexBuffer = new VulkanVertexBuffer(vertexBufferConfig);

        VulkanIndexBufferConfig indexBufferConfig{};
        indexBufferConfig.device = config.device;
        indexBufferConfig.commandPool = commandPool;
        indexBufferConfig.size = sizeof(indices[0]) * indices.size();
        indexBuffer = new VulkanIndexBuffer(indexBufferConfig);
    }

    Skybox::~Skybox() {
        delete indexBuffer;
        delete vertexBuffer;

        config.device->destroyGraphicsPipeline(pipeline);
        config.device->destroyPipelineLayout(pipelineLayout);

        for (int i = 0; i < uniformBuffers.size(); ++i) {
            delete uniformBuffers[i];
        }
        uniformBuffers.clear();

        config.device->destroyDescriptorSetLayout(descriptorSetLayout);
        config.device->destroyDescriptorPool(descriptorPool);
        config.device->destroySampler(textureSampler);
        config.device->destroyImageView(imageView);
        config.device->freeMemory(deviceMemory);
        config.device->destroyImage(image);

        destroyCommandPool();
    }

    void Skybox::render(const VulkanCommandBuffer& commandBuffer, uint32_t currentFrame) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

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
            pipelineLayout,
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
            (uint32_t) indices.size(),
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
        //uniformBufferData.projection[1][1] *= -1;

        uniformBuffer->setData(&uniformBufferData);
    }

    void Skybox::createGraphicsPipeline(std::shared_ptr<VulkanShader> vertexShader, std::shared_ptr<VulkanShader> fragmentShader) {
        VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo{};
        vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStageCreateInfo.module = *vertexShader;
        vertexShaderStageCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{};
        fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStageCreateInfo.module = *fragmentShader;
        fragmentShaderStageCreateInfo.pName = "main";

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {
                vertexShaderStageCreateInfo,
                fragmentShaderStageCreateInfo
        };

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT, // Indicates that the viewport is set dynamically with vkCmdSetViewport elsewhere
            VK_DYNAMIC_STATE_SCISSOR // Indicates that the scissor is set dynamically with vkCmdSetScissor elsewhere
        };

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = (uint32_t) dynamicStates.size();
        dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.scissorCount = 1;

        VkVertexInputBindingDescription vertexInputBindingDescription{};
        vertexInputBindingDescription.binding = 0;
        vertexInputBindingDescription.stride = sizeof(glm::vec3);
        vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription{
            {
                .binding = 0,
                .location = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = 0,
            }
        };

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputAttributeDescription.size();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
        inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationStateCreateInfo.lineWidth = 1.0f;
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencil.depthTestEnable = VK_FALSE;
        depthStencil.depthWriteEnable = VK_FALSE;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentState.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
        colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;

        VkPipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutCreateInfo.setLayoutCount = 1;
        layoutCreateInfo.pSetLayouts = &descriptorSetLayout;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createPipelineLayout(&layoutCreateInfo, &pipelineLayout));

        VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stageCount = shaderStages.size();
        pipelineCreateInfo.pStages = shaderStages.data();
        pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
        pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
        pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        pipelineCreateInfo.layout = pipelineLayout;
        pipelineCreateInfo.renderPass = config.swapChain->getRenderPass();
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.pDepthStencilState = &depthStencil;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createGraphicsPipeline(&pipelineCreateInfo, &pipeline));
    }

    void Skybox::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image, VkFormat format, VkCommandBuffer commandBuffer, VkImageSubresourceRange subresourceRange) const {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange = subresourceRange;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            BL_THROW("Unsupported layout transition");
        }

        constexpr VkDependencyFlags dependencyFlags = 0;
        constexpr uint32_t memoryBarrierCount = 0;
        constexpr VkMemoryBarrier* memoryBarriers = nullptr;
        constexpr uint32_t bufferMemoryBarrierCount = 0;
        constexpr VkBufferMemoryBarrier* bufferMemoryBarriers = nullptr;
        constexpr uint32_t imageMemoryBarrierCount = 1;
        vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage,
                destinationStage,
                dependencyFlags,
                memoryBarrierCount,
                memoryBarriers,
                bufferMemoryBarrierCount,
                bufferMemoryBarriers,
                imageMemoryBarrierCount,
                &barrier
        );
    }

    void Skybox::createCommandPool() {
        VulkanCommandPoolConfig commandPoolConfig{};
        commandPoolConfig.device = config.device;
        commandPool = new VulkanCommandPool(commandPoolConfig);
    }

    void Skybox::destroyCommandPool() const {
        delete commandPool;
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

    void Skybox::destroyDescriptorPool() const {
        config.device->destroyDescriptorPool(descriptorPool);
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

    void Skybox::destroyDescriptorSetLayout() const {
        config.device->destroyDescriptorSetLayout(descriptorSetLayout);
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
            VkDescriptorSet descriptorSet = descriptorSets[i];

            VkDescriptorBufferInfo descriptorBufferInfo{};
            descriptorBufferInfo.buffer = *uniformBuffers[i];
            descriptorBufferInfo.offset = 0;
            descriptorBufferInfo.range = sizeof(UniformBufferData);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSet;
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &descriptorBufferInfo;

            config.device->updateDescriptorSets(1, &descriptorWrite);

            // VkDescriptorBufferInfo uniformBufferDescriptorBufferInfo{};
            // uniformBufferDescriptorBufferInfo.buffer = *uniformBuffers[i];
            // uniformBufferDescriptorBufferInfo.offset = 0;
            // uniformBufferDescriptorBufferInfo.range = sizeof(UniformBufferData);
            //
            // VkWriteDescriptorSet uniformBufferDescriptorWrite{};
            // uniformBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            // uniformBufferDescriptorWrite.dstSet = descriptorSet;
            // uniformBufferDescriptorWrite.dstBinding = 0;
            // uniformBufferDescriptorWrite.dstArrayElement = 0;
            // uniformBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            // uniformBufferDescriptorWrite.descriptorCount = 1;
            // uniformBufferDescriptorWrite.pBufferInfo = &uniformBufferDescriptorBufferInfo;
            //
            // VkDescriptorImageInfo textureDescriptorImageInfo{};
            // textureDescriptorImageInfo.imageLayout = texture->getImageLayout();
            // textureDescriptorImageInfo.imageView = texture->getImageView();
            // textureDescriptorImageInfo.sampler = textureSampler;
            //
            // VkWriteDescriptorSet textureDescriptorWrite{};
            // textureDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            // textureDescriptorWrite.dstSet = descriptorSet;
            // textureDescriptorWrite.dstBinding = 1;
            // textureDescriptorWrite.dstArrayElement = 0;
            // textureDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            // textureDescriptorWrite.descriptorCount = 1;
            // textureDescriptorWrite.pImageInfo = &textureDescriptorImageInfo;
            //
            // std::array<VkWriteDescriptorSet, 2> descriptorWrites = {
            //     uniformBufferDescriptorWrite,
            //     textureDescriptorWrite,
            // };
            //
            // config.device->updateDescriptorSets(1, descriptorWrites.data());
        }
    }

    void Skybox::destroyDescriptorSets() const {
        // Destroyed when pool is destroyed
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

    void Skybox::destroyUniformBuffers() {
        for (int i = 0; i < uniformBuffers.size(); ++i) {
            delete uniformBuffers[i];
            uniformBuffers[i] = nullptr;
        }
    }

    void Skybox::createTextureSampler() {
        VulkanPhysicalDevice* physicalDevice = config.device->getPhysicalDevice();
        const VkPhysicalDeviceProperties& physicalDeviceProperties = physicalDevice->getProperties();

        VkSamplerCreateInfo textureSamplerCreateInfo{};
        textureSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        textureSamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        textureSamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        textureSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        textureSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        textureSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        textureSamplerCreateInfo.anisotropyEnable = VK_TRUE;
        textureSamplerCreateInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;
        textureSamplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        textureSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        textureSamplerCreateInfo.compareEnable = VK_FALSE;
        textureSamplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        textureSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        textureSamplerCreateInfo.mipLodBias = 0.0f;
        textureSamplerCreateInfo.minLod = 0.0f;
        textureSamplerCreateInfo.maxLod = 0.0f;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createSampler(&textureSamplerCreateInfo, &textureSampler));
    }

    void Skybox::destroyTextureSampler() const {
        config.device->destroySampler(textureSampler);
    }

    void Skybox::createTextures() {
        BL_LOG_DEBUG("Loading skybox textures");
        std::array<std::string, FACE_COUNT> images = {
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_back.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_bot.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_front.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_left.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_right.png",
            "models/oxar_freighter/Spaceboxes/Blue/bkg1_top.png"
        };
        for (int i = 0; i < images.size(); ++i) {
            std::shared_ptr<ImageFile> imageFile = config.fileSystem->readImage(images[i]);

            VulkanImageConfig textureConfig{};
            textureConfig.device = config.device;
            textureConfig.commandPool = commandPool;
            textureConfig.width = imageFile->width;
            textureConfig.height = imageFile->height;
            textureConfig.format = VK_FORMAT_R8G8B8A8_SRGB;
            textureConfig.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            textureConfig.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
            textureConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

            textures[i] = std::make_shared<VulkanImage>(textureConfig);
        }
        BL_LOG_INFO("Loaded [{}] skybox textures", textures.size());
        BL_ASSERT(textures.size() == images.size());
        BL_ASSERT(textures.size() == FACE_COUNT);
    }

    void Skybox::destroyTextures() {
        for (int i = 0; i < textures.size(); ++i) {
            textures[i] = nullptr;
        }
    }
}
