#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

struct SensorData {//Define la estructura de datos
    float temperature;
    float humidity;
    unsigned long timestamp;//guarda el tiempo que se tomo la lectura del clima
    bool valid;
};
#endif
