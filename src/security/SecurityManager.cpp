#include "SecurityManager.h"

// Calcula una firma FNV-1a de 32 bits sobre el payload y la retorna en hexadecimal.
// FNV-1a es liviano y sin dependencias externas, adecuado para microcontroladores.
String SecurityManager::generateSignature(const String& payload) const {
    uint32_t hash = 2166136261u;  // FNV offset basis

    for (size_t i = 0; i < payload.length(); ++i) {
        hash ^= static_cast<uint8_t>(payload[i]);
        hash *= 16777619u;        // FNV prime
    }

    // Formato hex con ceros a la izquierda para ancho fijo de 8 caracteres
    char buf[9];
    snprintf(buf, sizeof(buf), "%08" PRIX32, hash);
    return String(buf);
}