#ifndef ISENSOR_H
#define ISENSOR_H

#include "../utils/SensorData.h"
// es una plantilla  es como un contrato  que permite la abtraccion y facilita la integracion de nuevos dispositivos si alterar la logica del sistema principal
class ISensor {
public:
    virtual void begin() = 0;
    virtual SensorData read() = 0;
};

#endif
