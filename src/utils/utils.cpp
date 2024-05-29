#include "pch.h"
#include "utils/utils.h"

namespace Blink {

    // ChatGPT:
    // ---
    // In numerical computing, epsilon (often referred to as machine epsilon or floating-point epsilon) is a measure of
    // the smallest difference between two representable floating-point numbers. It gives an upper bound on the relative
    // error due to rounding in floating-point arithmetic.
    constexpr float EPSILON = 1e-7;

    // Use epsilon to check if a floating-point result is close enough to zero to be considered
    // zero. This is necessary because floating-point calculations can introduce very small errors.
    float clampToZero(float x) {
        if (abs(x) < EPSILON) {
            return 0;
        }
        return x;
    }

    void clampToZero(glm::vec3* vector) {
        if (abs(vector->x) < EPSILON) {
            vector->x = 0.0f;
        }
        if (abs(vector->y) < EPSILON) {
            vector->y = 0.0f;
        }
        if (abs(vector->z) < EPSILON) {
            vector->z = 0.0f;
        }
    }
}
