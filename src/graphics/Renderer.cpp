#include "Renderer.h"
#include "Vertex.h"
#include "system/Log.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Blink {
    Renderer::Renderer(
            IndexBuffer* indexBuffer,
            VertexBuffer* vertexBuffer,
            Shader* shader,
            Texture* whiteTexture
    ) : indexBuffer(indexBuffer),
        vertexBuffer(vertexBuffer),
        shader(shader),
        whiteTexture(whiteTexture),
        indices(new uint32_t[INDICES_PER_BATCH]),
        indexCount(0),
        vertices(new Vertex[VERTICES_PER_BATCH]),
        vertexCount(0),
        textures(new Texture* [MAX_TEXTURE_SLOTS]),
        textureCount(0),
        reservedTextureCount(0) {}

    Renderer::~Renderer() {
        delete[] textures;
        delete[] vertices;
        delete[] indices;
    }

    bool Renderer::initialize() {
        return true;
    }

    void Renderer::terminate() const {

    }

    void Renderer::beginFrame() {
    }

    void Renderer::endFrame() {
    }

    void Renderer::submitQuad(Quad& quad) {
    }

    void Renderer::draw() {
    }

    void Renderer::drawIndexed() const {
    }

    void Renderer::reset() {

    }
}

