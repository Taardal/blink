#pragma once

#include <numbers>

namespace Blink {
    struct IndexBufferData {
        uint32_t* indices = nullptr;
        uint32_t indexCount = 0;
        uint32_t usage = 0;
    };

    struct IndexBufferSubData {
        uint32_t* indices = nullptr;
        uint32_t indexCount = 0;
        uint32_t offset = 0;
    };
}

namespace Blink {
    class IndexBuffer {
    private:
        uint32_t id = 0;

    public:
        bool initialize();

        void terminate() const;

        void bind() const;

        void unbind() const;

        void setData(const IndexBufferData& data) const;

        void setSubData(const IndexBufferSubData& data) const;
    };
}
