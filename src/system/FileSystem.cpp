#include "pch.h"
#include "FileSystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <filesystem>
#include <fstream>

namespace Blink {
    bool FileSystem::exists(const std::string& path) const {
        return std::filesystem::exists(path);
    }

    std::vector<char> FileSystem::readBytes(const std::string& path) const {
        if (!exists(path)) {
            BL_THROW("Could not find file [" + path + "]");
        }
        std::ifstream file{path.c_str(), std::ios::ate | std::ios::binary};
        if (!file.is_open()) {
            BL_THROW("Could not open file with path [" + path + "]");
        }
        auto fileSize = (uint32_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }

    std::shared_ptr<ImageFile> FileSystem::readImage(const std::string& path) const {
        std::string imageFilepath = path;
        cleanPath(&imageFilepath);
        BL_LOG_DEBUG("Reading image file [{}]", imageFilepath);

        if (!exists(imageFilepath)) {
            BL_THROW("Could not find image file [" + imageFilepath + "]");
        }

        int32_t width;
        int32_t height;
        int32_t channels;
        int32_t desiredChannels = STBI_rgb_alpha;
        unsigned char* pixels = stbi_load(imageFilepath.c_str(), &width, &height, &channels, desiredChannels);
        if (!pixels) {
            BL_THROW("Could not read image file [" + imageFilepath + "]");
        }

        auto imageFile = std::make_shared<ImageFile>();
        imageFile->width = width;
        imageFile->height = height;
        imageFile->channels = channels;
        imageFile->size = width * height * desiredChannels;
        imageFile->pixels = pixels;
        return imageFile;
    }

    std::shared_ptr<ObjFile> FileSystem::readObj(const std::string& path) const {
        std::string objFilepath = path;
        cleanPath(&objFilepath);
        BL_LOG_DEBUG("Reading OBJ file [{}]", objFilepath);

        if (!exists(objFilepath)) {
            BL_THROW("Could not find OBJ file [" + objFilepath + "]");
        }

        // .mtl file should be located next to the .obj file
        std::string mtlDirectoryPath = objFilepath.substr(0, objFilepath.find_last_of("/") + 1);

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

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objFilepath.c_str(), mtlDirectoryPath.c_str(), triangulate)) {
            BL_THROW("Could not load OBJ file [" + objFilepath + "]: " + err);
        }

        auto objFile = std::make_shared<ObjFile>();
        objFile->attrib = attrib;
        objFile->shapes = shapes;
        objFile->materials = materials;
        return objFile;
    }

    void FileSystem::cleanPath(std::string* path) const {
        std::replace(path->begin(), path->end(), '\\', '/');
    }
}
