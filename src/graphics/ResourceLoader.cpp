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
        std::replace(texturePath.begin(), texturePath.end(), '\\', '/');

        int32_t width;
        int32_t height;
        int32_t channels;
        int32_t desiredChannels = STBI_rgb_alpha;

        unsigned char* pixels = stbi_load(texturePath.c_str(), &width, &height, &channels, desiredChannels);
        if (!pixels) {
            BL_THROW("Could not load texture [" + std::string(texturePath) + "]");
        }

        auto image = std::make_shared<Image>();
        image->width = width;
        image->height = height;
        image->channels = channels;
        image->size = width * height * desiredChannels;
        image->pixels = pixels;
        return image;
    }

    std::shared_ptr<Model> ResourceLoader::loadModel(const std::string& path, const std::string& name) const {
        BL_ASSERT(!path.empty());
        BL_ASSERT(!name.empty());

        std::string objFilePath = path + "/" + name + ".obj";
        std::replace(objFilePath.begin(), objFilePath.end(), '\\', '/');

        std::string mtlDirectoryPath = path + "/";
        std::replace(mtlDirectoryPath.begin(), mtlDirectoryPath.end(), '\\', '/');

        BL_LOG_DEBUG("Loading model [{}]", objFilePath);

        // An OBJ file consists of positions, normals, texture coordinates and faces.
        // Faces consist of an arbitrary amount of vertices, where each vertex refers to a position, normal and/or texture coordinate by index.
        // The attrib container holds all of the positions, normals and texture coordinates in its attrib.vertices, attrib.normals and attrib.texcoords vectors.
        // The shapes container contains all of the separate objects and their faces.
        // Each face consists of an array of vertices, and each vertex contains the indices of the position, normal and texture coordinate attributes.
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        bool triangulate = true;
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objFilePath.c_str(), mtlDirectoryPath.c_str(), triangulate)) {
            BL_THROW("Could not load model: " + err);
        }

        BL_LOG_DEBUG("Vertices [{}]", attrib.vertices.size());
        BL_LOG_DEBUG("Normals [{}]", attrib.normals.size());
        BL_LOG_DEBUG("Texcoords [{}]", attrib.texcoords.size());
        BL_LOG_DEBUG("Shapes [{}]", shapes.size());
        BL_LOG_DEBUG("Materials [{}]", materials.size());

        for (int i = 0; i < materials.size(); ++i) {
            auto& material = materials[i];
            BL_LOG_DEBUG("--------------------------------------------------------------------");
            BL_LOG_DEBUG("| {}", material.name);
            BL_LOG_DEBUG("--------------------------------------------------------------------");
            BL_LOG_DEBUG("| ambient_texname             | {}", material.ambient_texname);
            BL_LOG_DEBUG("| diffuse_texname             | {}", material.diffuse_texname);
            BL_LOG_DEBUG("| specular_texname            | {}", material.specular_texname);
            BL_LOG_DEBUG("| specular_highlight_texname  | {}", material.specular_highlight_texname);
            BL_LOG_DEBUG("| bump_texname                | {}", material.bump_texname);
            BL_LOG_DEBUG("| displacement_texname        | {}", material.displacement_texname);
            BL_LOG_DEBUG("| alpha_texname               | {}", material.alpha_texname);
        }

        //printModelInfo(attrib, shapes, materials);

        auto model = std::make_shared<Model>();

        model->attrib = attrib;
        model->shapes = shapes;
        model->materials = materials;

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

    void ResourceLoader::printModelInfo(
        const tinyobj::attrib_t& attrib,
        const std::vector<tinyobj::shape_t>& shapes,
        const std::vector<tinyobj::material_t>& materials
    ) const {
        std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
        std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
        std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2)
                << std::endl;

        std::cout << "# of shapes    : " << shapes.size() << std::endl;
        std::cout << "# of materials : " << materials.size() << std::endl;

        for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
            printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
                   static_cast<const double>(attrib.vertices[3 * v + 0]),
                   static_cast<const double>(attrib.vertices[3 * v + 1]),
                   static_cast<const double>(attrib.vertices[3 * v + 2]));
        }

        for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
            printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
                   static_cast<const double>(attrib.normals[3 * v + 0]),
                   static_cast<const double>(attrib.normals[3 * v + 1]),
                   static_cast<const double>(attrib.normals[3 * v + 2]));
        }

        for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
            printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
                   static_cast<const double>(attrib.texcoords[2 * v + 0]),
                   static_cast<const double>(attrib.texcoords[2 * v + 1]));
        }

        // For each shape
        for (size_t i = 0; i < shapes.size(); i++) {
            printf("shape[%ld].name = %s\n", static_cast<long>(i),
                   shapes[i].name.c_str());
            printf("Size of shape[%ld].mesh.indices: %lu\n", static_cast<long>(i),
                   static_cast<unsigned long>(shapes[i].mesh.indices.size()));
            // printf("Size of shape[%ld].lines.indices: %lu\n", static_cast<long>(i),
            //        static_cast<unsigned long>(shapes[i].lines.indices.size()));
            // printf("Size of shape[%ld].points.indices: %lu\n", static_cast<long>(i),
            //        static_cast<unsigned long>(shapes[i].points.indices.size()));

            size_t index_offset = 0;

            // assert(shapes[i].mesh.num_face_vertices.size() ==
            //     shapes[i].mesh.material_ids.size());
            //
            // assert(shapes[i].mesh.num_face_vertices.size() ==
            //     shapes[i].mesh.smoothing_group_ids.size());

            printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
                   static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

            // For each face
            for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
                size_t fnum = shapes[i].mesh.num_face_vertices[f];

                printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
                       static_cast<unsigned long>(fnum));

                // For each vertex in the face
                for (size_t v = 0; v < fnum; v++) {
                    tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
                    printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f),
                           static_cast<long>(v), idx.vertex_index, idx.normal_index,
                           idx.texcoord_index);
                }

                printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
                       shapes[i].mesh.material_ids[f]);
                // printf("  face[%ld].smoothing_group_id = %d\n", static_cast<long>(f),
                //        shapes[i].mesh.smoothing_group_ids[f]);

                index_offset += fnum;
            }

            printf("shape[%ld].num_tags: %lu\n", static_cast<long>(i),
                   static_cast<unsigned long>(shapes[i].mesh.tags.size()));
            for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
                printf("  tag[%ld] = %s ", static_cast<long>(t),
                       shapes[i].mesh.tags[t].name.c_str());
                printf(" ints: [");
                for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
                    printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
                    if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
                        printf(", ");
                    }
                }
                printf("]");

                printf(" floats: [");
                for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
                    printf("%f", static_cast<const double>(
                               shapes[i].mesh.tags[t].floatValues[j]));
                    if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
                        printf(", ");
                    }
                }
                printf("]");

                printf(" strings: [");
                for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
                    printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
                    if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
                        printf(", ");
                    }
                }
                printf("]");
                printf("\n");
            }
        }

        for (size_t i = 0; i < materials.size(); i++) {
            printf("material[%ld].name = %s\n", static_cast<long>(i),
                   materials[i].name.c_str());
            printf("  material.Ka = (%f, %f ,%f)\n",
                   static_cast<const double>(materials[i].ambient[0]),
                   static_cast<const double>(materials[i].ambient[1]),
                   static_cast<const double>(materials[i].ambient[2]));
            printf("  material.Kd = (%f, %f ,%f)\n",
                   static_cast<const double>(materials[i].diffuse[0]),
                   static_cast<const double>(materials[i].diffuse[1]),
                   static_cast<const double>(materials[i].diffuse[2]));
            printf("  material.Ks = (%f, %f ,%f)\n",
                   static_cast<const double>(materials[i].specular[0]),
                   static_cast<const double>(materials[i].specular[1]),
                   static_cast<const double>(materials[i].specular[2]));
            printf("  material.Tr = (%f, %f ,%f)\n",
                   static_cast<const double>(materials[i].transmittance[0]),
                   static_cast<const double>(materials[i].transmittance[1]),
                   static_cast<const double>(materials[i].transmittance[2]));
            printf("  material.Ke = (%f, %f ,%f)\n",
                   static_cast<const double>(materials[i].emission[0]),
                   static_cast<const double>(materials[i].emission[1]),
                   static_cast<const double>(materials[i].emission[2]));
            printf("  material.Ns = %f\n",
                   static_cast<const double>(materials[i].shininess));
            printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
            printf("  material.dissolve = %f\n",
                   static_cast<const double>(materials[i].dissolve));
            printf("  material.illum = %d\n", materials[i].illum);
            printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
            printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
            printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
            printf("  material.map_Ns = %s\n",
                   materials[i].specular_highlight_texname.c_str());
            printf("  material.map_bump = %s\n", materials[i].bump_texname.c_str());
            printf("    bump_multiplier = %f\n", static_cast<const double>(materials[i].bump_texopt.bump_multiplier));
            printf("  material.map_d = %s\n", materials[i].alpha_texname.c_str());
            printf("  material.disp = %s\n", materials[i].displacement_texname.c_str());
            printf("  <<PBR>>\n");
            printf("  material.Pr     = %f\n", static_cast<const double>(materials[i].roughness));
            printf("  material.Pm     = %f\n", static_cast<const double>(materials[i].metallic));
            printf("  material.Ps     = %f\n", static_cast<const double>(materials[i].sheen));
            printf("  material.Pc     = %f\n", static_cast<const double>(materials[i].clearcoat_thickness));
            printf("  material.Pcr    = %f\n", static_cast<const double>(materials[i].clearcoat_roughness));
            printf("  material.aniso  = %f\n", static_cast<const double>(materials[i].anisotropy));
            printf("  material.anisor = %f\n", static_cast<const double>(materials[i].anisotropy_rotation));
            printf("  material.map_Ke = %s\n", materials[i].emissive_texname.c_str());
            printf("  material.map_Pr = %s\n", materials[i].roughness_texname.c_str());
            printf("  material.map_Pm = %s\n", materials[i].metallic_texname.c_str());
            printf("  material.map_Ps = %s\n", materials[i].sheen_texname.c_str());
            printf("  material.norm   = %s\n", materials[i].normal_texname.c_str());
            std::map<std::string, std::string>::const_iterator it(
                materials[i].unknown_parameter.begin());
            std::map<std::string, std::string>::const_iterator itEnd(
                materials[i].unknown_parameter.end());

            for (; it != itEnd; it++) {
                printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
            }
            printf("\n");
        }
    }
}
