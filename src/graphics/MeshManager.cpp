#include "pch.h"
#include "MeshManager.h"
#include "graphics/VulkanImage.h"

namespace Blink {
    MeshManager::MeshManager(const MeshManagerConfig& config) : config(config) {
        createCommandPool();
        createDescriptorPool();
        createDescriptorSetLayout();
        createTextureSampler();
        createPlaceholderTexture();
    }

    MeshManager::~MeshManager() {
        destroyTextureSampler();
        destroyDescriptorSetLayout();
        destroyDescriptorPool();
        destroyCommandPool();
    }

    VkDescriptorSetLayout MeshManager::getDescriptorSetLayout() const {
        return descriptorSetLayout;
    }

    std::shared_ptr<Mesh> MeshManager::getMesh(const MeshInfo& meshInfo) {
        auto mesh = std::make_shared<Mesh>();

        std::shared_ptr<ObjFile> objFile = getObjFile(meshInfo.modelPath);

        // Ensure that duplicate vertices are not added to the mesh for optimization purposes
        std::unordered_map<Vertex, uint32_t> indicesByUniqueVertices{};

        // Shapes of the model
        for (uint32_t s = 0; s < objFile->shapes.size(); s++) {
            tinyobj::shape_t& shape = objFile->shapes[s];
            uint32_t indexOffset = 0;

            // Faces (polygons) of the shape
            for (uint32_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                uint32_t vertexCountPerFace = shape.mesh.num_face_vertices[f];
                uint32_t materialId = shape.mesh.material_ids[f];

                // Vertices in the face
                for (uint32_t v = 0; v < vertexCountPerFace; v++) {
                    tinyobj::index_t index = shape.mesh.indices[indexOffset + v];

                    Vertex vertex{};
                    vertex.color = {1.0f, 1.0f, 1.0f};
                    if (materialId != -1) {
                        vertex.textureIndex = materialId;
                    }
                    vertex.position = {
                        objFile->attrib.vertices[3 * index.vertex_index + 0],
                        objFile->attrib.vertices[3 * index.vertex_index + 1],
                        objFile->attrib.vertices[3 * index.vertex_index + 2]
                    };
                    vertex.textureCoordinate = {
                        objFile->attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - objFile->attrib.texcoords[2 * index.texcoord_index + 1]
                    };

                    // Check if the current vertex has already been added
                    if (indicesByUniqueVertices.count(vertex) == 0) {
                        // Add the new vertex and use its index in the vertices vector as the rendering index
                        indicesByUniqueVertices[vertex] = (uint32_t) mesh->vertices.size();
                        mesh->vertices.push_back(vertex);
                    }
                    // Add the rendering index of the vertex, regardless if the vertex has already been added or not,
                    // to ensure that the index used to reference vertices in the mesh remains consistent even if some vertices are repeated.
                    mesh->indices.push_back(indicesByUniqueVertices[vertex]);
                }
                indexOffset += vertexCountPerFace;
            }
        }

        VulkanVertexBufferConfig vertexBufferConfig{};
        vertexBufferConfig.device = config.device;
        vertexBufferConfig.commandPool = commandPool;
        vertexBufferConfig.size = sizeof(mesh->vertices[0]) * mesh->vertices.size();

        auto vertexBuffer = std::make_shared<VulkanVertexBuffer>(vertexBufferConfig);
        vertexBuffer->setData(mesh->vertices);
        mesh->vertexBuffer = vertexBuffer;

        VulkanIndexBufferConfig indexBufferConfig{};
        indexBufferConfig.device = config.device;
        indexBufferConfig.commandPool = commandPool;
        indexBufferConfig.size = sizeof(mesh->indices[0]) * mesh->indices.size();

        auto indexBuffer = std::make_shared<VulkanIndexBuffer>(indexBufferConfig);
        indexBuffer->setData(mesh->indices);
        mesh->indexBuffer = indexBuffer;

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->allocateDescriptorSets(&descriptorSetAllocateInfo, &mesh->descriptorSet));

        for (uint32_t i = 0; i < MAX_TEXTURES_PER_MESH; ++i) {
            std::string textureFilepath;
            if (meshInfo.textureAtlasPath.size() > 0) {
                textureFilepath = meshInfo.textureAtlasPath;
            } else if (i < objFile->materials.size()) {
                tinyobj::material_t& material = objFile->materials[i];
                if (material.diffuse_texname.size() > 0) {
                    textureFilepath = meshInfo.texturesDirectoryPath + "/" + material.diffuse_texname;
                }
            }

            std::shared_ptr<VulkanImage> texture = nullptr;
            if (textureFilepath.size() == 0) {
                texture = placeholderTexture;
            } else {
                std::shared_ptr<ImageFile> imageFile = getImageFile(textureFilepath);
                texture = createTexture(imageFile);
            }
            mesh->textures.push_back(texture);

            VkDescriptorImageInfo descriptorImageInfo{};
            descriptorImageInfo.imageLayout = texture->getImageLayout();
            descriptorImageInfo.imageView = texture->getImageView();
            descriptorImageInfo.sampler = textureSampler;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = mesh->descriptorSet;
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = i;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &descriptorImageInfo;

            config.device->updateDescriptorSets(1, &descriptorWrite);
        }
        return mesh;
    }

    void MeshManager::resetDescriptors() {
        destroyDescriptorPool();
        createDescriptorPool();
    }

    std::shared_ptr<ObjFile> MeshManager::getObjFile(const std::string& path) {
        const auto iterator = objCache.find(path);
        if (iterator != objCache.end()) {
            return iterator->second;
        }
        std::shared_ptr<ObjFile> objFile = config.fileSystem->readObj(path);
        objCache[path] = objFile;
        return objFile;
    }

    std::shared_ptr<ImageFile> MeshManager::getImageFile(const std::string& path) {
        const auto iterator = imageCache.find(path);
        if (iterator != imageCache.end()) {
            return iterator->second;
        }
        std::shared_ptr<ImageFile> imageFile = config.fileSystem->readImage(path);
        imageCache[path] = imageFile;
        return imageFile;
    }

    std::shared_ptr<VulkanImage> MeshManager::createTexture(const std::shared_ptr<ImageFile>& imageFile) const {
        VulkanImageConfig textureConfig = {};
        textureConfig.device = config.device;
        textureConfig.commandPool = commandPool;
        textureConfig.width = imageFile->width;
        textureConfig.height = imageFile->height;
        textureConfig.format = VK_FORMAT_R8G8B8A8_SRGB;
        textureConfig.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureConfig.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
        textureConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        auto texture = std::make_shared<VulkanImage>(textureConfig);
        texture->setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        texture->setData(imageFile);
        texture->setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        return texture;
    }

    void MeshManager::createCommandPool() {
        VulkanCommandPoolConfig commandPoolConfig{};
        commandPoolConfig.device = config.device;
        commandPool = new VulkanCommandPool(commandPoolConfig);
    }

    void MeshManager::destroyCommandPool() const {
        delete commandPool;
    }

    void MeshManager::createDescriptorPool() {
        VkDescriptorPoolSize descriptorPoolSize{};
        descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorPoolSize.descriptorCount = MAX_TEXTURES;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = 1;
        descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;
        descriptorPoolCreateInfo.maxSets = MAX_MESHES;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createDescriptorPool(&descriptorPoolCreateInfo, &descriptorPool));
    }

    void MeshManager::destroyDescriptorPool() const {
        config.device->destroyDescriptorPool(descriptorPool);
    }

    void MeshManager::createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding textureSamplerLayoutBinding{};
        textureSamplerLayoutBinding.binding = 0;
        textureSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureSamplerLayoutBinding.descriptorCount = MAX_TEXTURES_PER_MESH;
        textureSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = 1;
        descriptorSetLayoutCreateInfo.pBindings = &textureSamplerLayoutBinding;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, &descriptorSetLayout));
    }

    void MeshManager::destroyDescriptorSetLayout() const {
        config.device->destroyDescriptorSetLayout(descriptorSetLayout);
    }

    void MeshManager::createTextureSampler() {
        VulkanPhysicalDevice* physicalDevice = config.device->getPhysicalDevice();
        const VkPhysicalDeviceProperties& physicalDeviceProperties = physicalDevice->getProperties();

        VkSamplerCreateInfo textureSamplerCreateInfo{};
        textureSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        textureSamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        textureSamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        textureSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        textureSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        textureSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        textureSamplerCreateInfo.anisotropyEnable = VK_TRUE;
        textureSamplerCreateInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;
        textureSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        textureSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        textureSamplerCreateInfo.compareEnable = VK_FALSE;
        textureSamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        textureSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        textureSamplerCreateInfo.mipLodBias = 0.0f;
        textureSamplerCreateInfo.minLod = 0.0f;
        textureSamplerCreateInfo.maxLod = 0.0f;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createSampler(&textureSamplerCreateInfo, &textureSampler));
    }

    void MeshManager::destroyTextureSampler() {
        config.device->destroySampler(textureSampler);
    }

    void MeshManager::createPlaceholderTexture() {
        auto placeholderTextureImage = std::make_shared<ImageFile>();
        placeholderTextureImage->width = 1;
        placeholderTextureImage->height = 1;
        placeholderTextureImage->channels = 4; // RGBA
        placeholderTextureImage->size = placeholderTextureImage->width * placeholderTextureImage->height * placeholderTextureImage->channels;
        placeholderTextureImage->pixels = new unsigned char[placeholderTextureImage->size];
        placeholderTextureImage->pixels[0] = 0; // Red
        placeholderTextureImage->pixels[1] = 0; // Green
        placeholderTextureImage->pixels[2] = 0; // Blue
        placeholderTextureImage->pixels[3] = 0; // Alpha (transparent)
        placeholderTexture = createTexture(placeholderTextureImage);
    }
}
