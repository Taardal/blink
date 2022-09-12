#pragma once

#include "IndexBuffer.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Quad.h"
#include <glm/glm.hpp>
#include <string>

namespace Blink {

    class Renderer {
    private:
        static const uint32_t VERTICES_PER_QUAD = 4;
        static const uint32_t INDICES_PER_QUAD = 6;
        static const uint32_t QUADS_PER_BATCH = 1000;
        static const uint32_t VERTICES_PER_BATCH = QUADS_PER_BATCH * VERTICES_PER_QUAD;
        static const uint32_t INDICES_PER_BATCH = QUADS_PER_BATCH * INDICES_PER_QUAD;
        static const uint32_t MAX_TEXTURE_SLOTS = 16;

    private:
        IndexBuffer* indexBuffer;
        VertexBuffer* vertexBuffer;
        Shader* shader;
        Texture* whiteTexture;
        uint32_t* indices;
        uint32_t indexCount;
        Vertex* vertices;
        uint32_t vertexCount;
        Texture** textures;
        uint32_t textureCount;
        uint32_t reservedTextureCount;

    public:
        Renderer(
                IndexBuffer* indexBuffer,
                VertexBuffer* vertexBuffer,
                Shader* shader,
                Texture* whiteTexture
        );

        ~Renderer();

        bool initialize();

        void terminate() const;

        void beginFrame();

        void endFrame();

        void submitQuad(Quad& quad);

    private:
        void draw();

        void drawIndexed() const;

        void reset();
    };
}


