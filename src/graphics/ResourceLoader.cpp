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

    std::shared_ptr<Image> ResourceLoader::loadTexture(const std::string& path) const {
        BL_ASSERT(!path.empty());

        std::string texturePath = path;
        cleanPath(&texturePath);
        BL_LOG_DEBUG("Loading texture [{}]", texturePath);

        int32_t width;
        int32_t height;
        int32_t channels;
        int32_t desiredChannels = STBI_rgb_alpha;

        unsigned char* pixels = stbi_load(texturePath.c_str(), &width, &height, &channels, desiredChannels);
        if (!pixels) {
            BL_THROW("Could not load texture [" + texturePath + "]");
        }

        auto image = std::make_shared<Image>();
        image->width = width;
        image->height = height;
        image->channels = channels;
        image->size = width * height * desiredChannels;
        image->pixels = pixels;
        return image;
    }

    std::shared_ptr<Model> ResourceLoader::loadModel(const MeshConfig& meshConfig) const {
        std::string objFilePath = meshConfig.modelPath;
        cleanPath(&objFilePath);
        BL_LOG_DEBUG("Loading model [{}]", objFilePath);

        size_t findLastOf = objFilePath.find_last_of("/");
        std::string mtlDirectoryPath = objFilePath.substr(0, findLastOf + 1);
        BL_LOG_DEBUG("Loading model MTL [{}]", mtlDirectoryPath);

        // objFilePath = "models/sibenik/sibenik.obj";
        // mtlDirectoryPath = "models/sibenik/";

        // An OBJ file consists of positions, normals, texture coordinates and faces.
        // Faces consist of an arbitrary amount of vertices, where each vertex refers to a position, normal and/or texture coordinate by index.
        // The attrib container holds all of the positions, normals and texture coordinates in its attrib.vertices, attrib.normals and attrib.texcoords vectors.
        // The shapes container contains all of the separate objects and their faces.
        // Each face consists of an array of vertices, and each vertex contains the indices of the position, normal and texture coordinate attributes.
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        // https://github.com/tinyobjloader/tinyobjloader?tab=readme-ov-file#robust-triangulation
        // When you enable triangulation(default is enabled), TinyObjLoader triangulate polygons(faces with 4 or more vertices).
        constexpr bool triangulate = true;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objFilePath.c_str(), mtlDirectoryPath.c_str(), triangulate)) {
            BL_THROW("Could not load model [" + objFilePath + "]: " + err);
        }

        auto model = std::make_shared<Model>();

        model->attrib = attrib;
        model->shapes = shapes;
        model->materials = materials;

        // Loop over shapes
        for (size_t shapeIndex = 0; shapeIndex < shapes.size(); shapeIndex++) {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            tinyobj::shape_t& shape = shapes[shapeIndex];
            for (size_t faceIndex = 0; faceIndex < shape.mesh.num_face_vertices.size(); faceIndex++) {
                size_t faceVertexIndex = (size_t) shape.mesh.num_face_vertices[faceIndex];

                int materialId = shape.mesh.material_ids[faceIndex];

                // Loop over vertices in the face.
                for (size_t vertexIndex = 0; vertexIndex < faceVertexIndex; vertexIndex++) {
                    Vertex vertex{};
                    vertex.color = {1.0f, 1.0f, 1.0f};

                    if (materialId != -1) {
                        vertex.textureIndex = materialId;
                    }

                    // access to vertex
                    tinyobj::index_t index = shape.mesh.indices[index_offset + vertexIndex];

                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.textureCoordinate = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                    model->vertices.push_back(vertex);
                    model->indices.push_back(index_offset + vertexIndex);
                }
                index_offset += faceVertexIndex;
            }
        }

        return model;
    }

    void ResourceLoader::cleanPath(std::string* path) const {
        std::replace(path->begin(), path->end(), '\\', '/');
    }
}
