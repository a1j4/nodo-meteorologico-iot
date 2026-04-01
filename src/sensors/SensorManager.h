#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H
#include"DHTSensor.h"
#include "ISensor.h"

#include "../utils/SensorData.h"

class SensorManager {
private:
    bool initialized;
    DHTSensor dht;

public:
    SensorManager();
    void begin();
    SensorData readSensors();
};

#endif
