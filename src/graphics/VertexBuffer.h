#pragma once

#include "Vertex.h"
#include "VertexAttribute.h"
#include <numbers>
#include <vector>

namespace Blink {
    struct VertexBufferLayout {
        std::vector<VertexAttribute> attributes;
        uint32_t stride = 0;

        VertexBufferLayout(const std::initializer_list<VertexAttribute>& attributes);

        std::vector<VertexAttribute>::const_iterator begin() const;

        std::vector<VertexAttribute>::const_iterator end() const;
    };
}

namespace Blink {
    class VertexBuffer {
    private:
        uint32_t id = 0;
        VertexBufferLayout layout{};

    public:
        const VertexBufferLayout& getLayout() const;

        void setLayout(const VertexBufferLayout& layout);

        bool initialize();

        void terminate() const;
    };
}
