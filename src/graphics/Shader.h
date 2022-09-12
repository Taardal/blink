#pragma once

#include <string>

namespace Blink {
    class Shader {
    private:
        uint32_t id = 0;

    public:
        bool initialize(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);

        void terminate() const;
    };
}
