#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include "../utils/SensorData.h"
//Recibir datos y devolverá datos procesados
class DataProcessor{
    public:
    SensorData validate(const SensorData& data);
};
#endif