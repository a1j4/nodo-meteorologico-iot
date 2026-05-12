#include "SecurityManager.h"

String SecurityManager::generateSignature(String payload) {

    int hash = 0;

    for (int i = 0; i < payload.length(); i++) {
        hash += payload[i] * (i + 1);
    }

    return String(hash);
}