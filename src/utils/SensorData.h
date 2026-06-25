#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <stdint.h>

// Estructura que representa una lectura del sensor en un instante dado
struct SensorData {
    float    temperature;  // Temperatura en grados Celsius
    float    humidity;     // Humedad relativa en porcentaje
    uint32_t timestamp;    // Tiempo de lectura en unixtime (desde el RTC)
    bool     valid;        // false si la lectura es inválida o está vacía
};

#endif
