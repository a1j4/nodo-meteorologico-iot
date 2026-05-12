#include <Arduino.h>

#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include "../utils/SensorData.h"

#define BUFFER_SIZE 10

// 1. Las variables RTC deben estar FUERA de la clase para que funcionen
extern RTC_DATA_ATTR SensorData rtc_buffer[BUFFER_SIZE];
extern RTC_DATA_ATTR int rtc_index;
extern RTC_DATA_ATTR int rtc_count;

class DataBuffer {
public:
    DataBuffer();
    void add(const SensorData &data);
    SensorData get(int position);
    int size();
};

#endif