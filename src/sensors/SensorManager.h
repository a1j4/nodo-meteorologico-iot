#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "../utils/SensorData.h"
#include "DHTSensor.h"

// Coordina la inicialización y lectura de todos los sensores del nodo
class SensorManager {
private:
    DHTSensor dht;

public:
    void begin();
    SensorData readSensors();
};

#endif
