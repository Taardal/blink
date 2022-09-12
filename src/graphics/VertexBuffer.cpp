#include "VertexBuffer.h"

#include "utils/size.h"

namespace Blink {
    VertexBufferLayout::VertexBufferLayout(const std::initializer_list<VertexAttribute>& attributes) : attributes(attributes) {
        for (VertexAttribute& attribute: this->attributes) {
            attribute.offset = stride;
            stride += attribute.size;
        }
    }

    std::vector<VertexAttribute>::const_iterator VertexBufferLayout::begin() const {
        return attributes.begin();
    }

    std::vector<VertexAttribute>::const_iterator VertexBufferLayout::end() const {
        return attributes.end();
    }
}

namespace Blink {

    const VertexBufferLayout& VertexBuffer::getLayout() const {
        return layout;
    }

    void VertexBuffer::setLayout(const VertexBufferLayout& layout) {
        this->layout = layout;
    }

    bool VertexBuffer::initialize() {
        return true;
    }

    void VertexBuffer::terminate() const {
    }
}