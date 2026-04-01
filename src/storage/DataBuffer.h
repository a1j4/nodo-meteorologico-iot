#include <Arduino.h>
#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H
#include "../utils/SensorData.h"
#define BUFFER_SIZE 10

class DataBuffer
{
private:
    //SensorData buffer [BUFFER_SIZE];
    RTC_DATA_ATTR SensorData buffer[BUFFER_SIZE];
    RTC_DATA_ATTR int index;
    RTC_DATA_ATTR int count;
    int index;
    int count;

public:
    DataBuffer();
    void add(const SensorData &data);
    SensorData get(int position);
    int size();
};
#endif