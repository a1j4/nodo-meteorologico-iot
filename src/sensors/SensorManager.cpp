#include "SensorManager.h"

void SensorManager::begin() {
    dht.begin();
}

// Delega la lectura al DHTSensor; el timestamp lo asigna sampleAndFilter() con el RTC
SensorData SensorManager::readSensors() {
    return dht.read();
}