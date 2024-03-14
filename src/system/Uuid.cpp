#include "Uuid.h"
#include "Random.h"

namespace Blink {
    std::string Uuid::create() {
        const std::string hex_chars = "0123456789ABCDEF";
        std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
        for (int i = 0; i < uuid.size(); ++i) {
            if (uuid[i] == 'x') {
                uuid[i] = hex_chars[Random::getIntBetween(0, 15)];
            } else if (uuid[i] == 'y') {
                uuid[i] = hex_chars[Random::getIntBetween(8, 11)];
            }
        }
        return uuid;
    }
}
