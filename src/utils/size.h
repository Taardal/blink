#pragma once

#include <numbers>

template<typename T>
uint32_t sizeOf(uint32_t count) {
    return count * sizeof(T);
}