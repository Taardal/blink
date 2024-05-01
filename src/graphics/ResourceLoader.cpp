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
        std::shared_ptr<ImageFile> imageFile = config.fileSystem->readImage(path);

        auto image = std::make_shared<Image>();
        image->width = imageFile->width;
        image->height = imageFile->height;
        image->channels = imageFile->channels;
        image->size = imageFile->size;
        image->pixels = imageFile->pixels;
        return image;
    }

    std::shared_ptr<Model> ResourceLoader::loadModel(const MeshConfig& meshConfig) const {
        std::shared_ptr<ObjFile> objFile = config.fileSystem->readObj(meshConfig.modelPath);

        auto model = std::make_shared<Model>();

        model->attrib = objFile->attrib;
        model->shapes = objFile->shapes;
        model->materials = objFile->materials;

        // Loop over shapes
        for (size_t shapeIndex = 0; shapeIndex < model->shapes.size(); shapeIndex++) {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            tinyobj::shape_t& shape = model->shapes[shapeIndex];
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
                        model->attrib.vertices[3 * index.vertex_index + 0],
                        model->attrib.vertices[3 * index.vertex_index + 1],
                        model->attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.textureCoordinate = {
                        model->attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - model->attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                    model->vertices.push_back(vertex);
                    model->indices.push_back(index_offset + vertexIndex);
                }
                index_offset += faceVertexIndex;
            }
        }

        return model;
    }
}
