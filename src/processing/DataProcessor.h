#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include "../utils/SensorData.h"

// Valida que una lectura sea físicamente coherente antes de procesarla
class DataProcessor {
public:
    SensorData validate(const SensorData& data) const;
};

#endif