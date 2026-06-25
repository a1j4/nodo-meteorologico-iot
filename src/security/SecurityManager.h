#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include <Arduino.h>

// Genera una firma de integridad para el payload antes de transmitirlo por LoRa
class SecurityManager {
public:
    String generateSignature(const String& payload) const;
};

#endif