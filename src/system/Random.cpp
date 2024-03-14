#include "Random.h"
#include <random>

namespace Blink {
    Random::Random() : engine(randomDevice()) {
    }

    Random& Random::get() {
        static Random instance;
        return instance;
    }
}