#include "SensorManager.h"
#include <Arduino.h>

SensorManager::SensorManager() {
    initialized = false;
}

void SensorManager::begin() {
    dht.begin();       // ✅ Inicializar sensor aquí (correcto)
    initialized = true;
}

SensorData SensorManager::readSensors() {

    SensorData data;

    if (!initialized) {
        data.valid = false;
        return data;
    }

    // ✅ Leer desde el sensor real (o simulado por ahora)
    data = dht.read();

    return data;
}