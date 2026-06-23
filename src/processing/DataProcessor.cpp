#include "DataProcessor.h"
#include <Arduino.h>

//Modulo de validaciones de datos, deside si la medicion es valida o no antes de usarla 
SensorData DataProcessor::validate(const SensorData& data){
    SensorData processed = data;

    if (!data.valid || isnan(data.temperature) || isnan(data.humidity)) {
        processed.valid = false;
        return processed;
    }

    processed.valid = true;

    //Validación de temperatura
    if (data.temperature < -10 || data.temperature > 60){
        processed.valid = false;
    }
    //Validación de humedad
    if (data.humidity < 0 || data.humidity > 100){
        processed.valid = false;
    }
    return processed;
}
