#ifndef ISENSOR_H
#define ISENSOR_H

#include "../utils/SensorData.h"

class ISensor {
public:
    virtual void begin() = 0;
    virtual SensorData read() = 0;
};

#endif