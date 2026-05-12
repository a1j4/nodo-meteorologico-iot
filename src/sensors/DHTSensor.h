#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "ISensor.h"
#include <DHT.h>
#include "../utils/Config.h"

class DHTSensor : public ISensor {

private:
    DHT _dht;

public:
    // Constructor
    DHTSensor();

    // Inicializa el sensor
    void begin() override;

    // Lee los datos del sensor
    SensorData read() override;
};

#endif
