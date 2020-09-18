#include "Renderer.h"
#include "Log.h"

namespace blink
{
    void Renderer::Submit(const Quad& quad) const
    {
        ST_LOG_TRACE(ST_TAG, "Submitted quad at [{0}, {1}]...", quad.Position.x, quad.Position.y);
    }
}

