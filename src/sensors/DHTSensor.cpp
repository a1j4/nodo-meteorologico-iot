#include "DHTSensor.h"
#include <Arduino.h>

void DHTSensor::begin() {
    // Aquí irá la inicialización real cuando compres el sensor
}

SensorData DHTSensor::read() {

    SensorData data;
    // Por ahora marcamos como inválido
    data.temperature = 0;
    data.humidity = 0;
    data.pressure = 0;
    data.timestamp = millis();
    data.valid = false;

    return data;
}