#include "SensorManager.h"

SensorManager::SensorManager() {
}

void SensorManager::begin() {
    dht.begin();
}

SensorData SensorManager::readSensors() {

    SensorData data = dht.read();

    data.timestamp = millis();

    return data;
}