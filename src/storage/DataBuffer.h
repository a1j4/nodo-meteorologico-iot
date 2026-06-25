#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include <Arduino.h>
#include "../utils/SensorData.h"

#define BUFFER_SIZE 10

// Variables en memoria RTC: sobreviven al deep sleep, declaradas fuera de la clase
extern RTC_DATA_ATTR SensorData rtc_buffer[BUFFER_SIZE];
extern RTC_DATA_ATTR int rtc_index;
extern RTC_DATA_ATTR int rtc_count;

// Buffer circular respaldado en memoria RTC
class DataBuffer {
public:
    void add(const SensorData& data);   // Inserta dato en la posición actual
    SensorData get(int position) const; // Retorna dato por índice lógico (0 = más antiguo)
    int size() const;                   // Cantidad de datos almacenados
};

#endif