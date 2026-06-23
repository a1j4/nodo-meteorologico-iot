#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include <Arduino.h>

class SecurityManager {

public:
    String generateSignature(const String& payload);

};

#endif