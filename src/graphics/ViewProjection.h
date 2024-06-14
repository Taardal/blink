#pragma once

namespace Blink {
    struct ViewProjection {
        glm::mat4 view;
        glm::mat4 projection;
    };
}

namespace Blink {
    struct ViewProjectionUniformBufferData {
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 projection;
    };
}