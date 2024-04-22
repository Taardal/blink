#include "pch.h"
#include "ResourceLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Blink {
    const std::string ResourceLoader::MODELS_PATH = "models";
    const std::string ResourceLoader::SHADERS_PATH = "shaders";
    const std::string ResourceLoader::TEXTURES_PATH = "textures";

    ResourceLoader::ResourceLoader(const ResourceLoaderConfig& config) : config(config) {
    }

    std::shared_ptr<Image> ResourceLoader::loadTexture(const std::string& name) const {
        BL_ASSERT(!name.empty());
        std::string path = TEXTURES_PATH + "/" + name;

        int32_t width;
        int32_t height;
        int32_t channels;
        int32_t desiredChannels = STBI_rgb_alpha;

        unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, desiredChannels);
        if (!pixels) {
            BL_THROW("Could not load texture [" + std::string(path) + "]");
        }

        auto image = std::make_shared<Image>();
        image->width = width;
        image->height = height;
        image->channels = channels;
        image->size = width * height * desiredChannels;
        image->pixels = pixels;
        return image;
    }

    std::shared_ptr<Model> ResourceLoader::loadModel(const std::string& name) const {
        BL_ASSERT(!name.empty());
        std::string path = MODELS_PATH + "/" + name;

        // An OBJ file consists of positions, normals, texture coordinates and faces.
        // Faces consist of an arbitrary amount of vertices, where each vertex refers to a position, normal and/or texture coordinate by index.
        // The attrib container holds all of the positions, normals and texture coordinates in its attrib.vertices, attrib.normals and attrib.texcoords vectors.
        // The shapes container contains all of the separate objects and their faces.
        // Each face consists of an array of vertices, and each vertex contains the indices of the position, normal and texture coordinate attributes.
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) {
            BL_THROW("Could not load model: " + err);
        }

        auto model = std::make_shared<Model>();
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const tinyobj::shape_t& shape : shapes) {
            for (const tinyobj::index_t& index : shape.mesh.indices) {

                Vertex vertex{};
                vertex.color = {1.0f, 1.0f, 1.0f};

                // Unfortunately the attrib.vertices array is an array of float values instead of something like glm::vec3, so we need to multiply the index by 3.
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
            };
                // Similarly, there are two texture coordinate components per entry.
                vertex.textureCoordinate = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    // The OBJ format assumes a coordinate system where a vertical coordinate of 0 means the bottom of the image,
                    // however we've uploaded our image into Vulkan in a top to bottom orientation where 0 means the top of the image.
                    // This can be solved by flipping the vertical component of the texture coordinates
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };
                // The offsets of 0, 1 and 2 are used to access the X, Y and Z components,
                // or the U and V components in the case of texture coordinates.
                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = (uint32_t) model->vertices.size();
                    model->vertices.push_back(vertex);
                }
                model->indices.push_back(uniqueVertices[vertex]);
            }
        }
        return model;
    }
}
