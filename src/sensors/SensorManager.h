#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H
#include"DHTSensor.h"
#include "ISensor.h"

#include "../utils/SensorData.h"
//inicializa los sensores 
class SensorManager {//se encarga de leer los sensores y devolver los datos
private:
    bool initialized;
    DHTSensor dht;

public:
    SensorManager();
    void begin();
    SensorData readSensors();
};

#endif
