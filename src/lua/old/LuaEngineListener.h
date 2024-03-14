#pragma once

#include <glm/glm.hpp>

namespace Blink
{
    class LuaEngineListener
    {
    public:
        virtual void OnDrawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) = 0;
    };
}
