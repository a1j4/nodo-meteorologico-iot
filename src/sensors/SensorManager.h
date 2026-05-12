#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "../utils/SensorData.h"
#include "DHTSensor.h"

class SensorManager {

private:
    DHTSensor dht;

public:
    SensorManager();

    void begin();

    SensorData readSensors();
};

#endif
