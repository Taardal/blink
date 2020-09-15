#pragma once

#include "glm/glm.hpp"

namespace blink
{
    class Renderer
    {
    public:
        struct Quad
        {
            glm::vec2 Position;
            glm::vec2 Size;
            glm::vec4 Color;
        };

        void Submit(const Quad& quad) const;
    };
}


