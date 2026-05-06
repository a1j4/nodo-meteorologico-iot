#include "DataProcessor.h"

//Modulo de validaciones de datos, deside si la medicion es valida o no antes de usarla 
SensorData DataProcessor::validate(const SensorData& data){
    SensorData processed = data;//Copiamos los datos originales
    processed.valid= true;//Asumimos que es válido

    //Validación de temperatura
    if (data.temperature<-10||data.temperature>60){
        processed.valid=false;
    }
    //Validación de humedad
    if (data.humidity<0||data.humidity>100){
        processed.valid=false;
    }
    //Validación de presión
    if (data.pressure<800||data.pressure>1200){
        processed.valid=false;
    }
    return processed;
}