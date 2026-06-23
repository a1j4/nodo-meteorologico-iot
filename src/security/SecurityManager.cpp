#include "SecurityManager.h"

String SecurityManager::generateSignature(const String& payload) {

    uint32_t hash = 2166136261u;

    for (size_t i = 0; i < payload.length(); ++i) {
        hash ^= static_cast<uint8_t>(payload[i]);
        hash *= 16777619u;
    }

    return String(hash);
}