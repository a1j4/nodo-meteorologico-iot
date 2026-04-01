#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "ISensor.h"

class DHTSensor : public ISensor {
public:
    void begin() ;
    SensorData read() ;
};

#endif