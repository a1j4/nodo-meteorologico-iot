#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "ISensor.h"
#include <DHT.h>
#include "../utils/Config.h"

class DHTSensor : public ISensor {
private:
    DHT _dht;

public:
    DHTSensor();
    void begin() override;
    SensorData read() override;
};

#endif
