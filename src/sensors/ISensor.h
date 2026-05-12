#ifndef ISENSOR_H
#define ISENSOR_H

#include "../utils/SensorData.h"

// Interfaz (Contrato) para sensores
class ISensor {
public:
    // Destructor virtual: importante para que la memoria se limpie bien
    virtual ~ISensor() {} 

    virtual void begin() = 0;
    virtual SensorData read() = 0;
};

#endif
