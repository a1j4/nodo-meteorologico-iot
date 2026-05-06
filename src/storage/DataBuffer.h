#include <Arduino.h>
#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H
#include "../utils/SensorData.h"
#define BUFFER_SIZE 10// dastos guardados maximo 10

class DataBuffer// guarda  varias mediciones de sensores en memoria
{
private:
    //SensorData buffer [BUFFER_SIZE];
    RTC_DATA_ATTR SensorData buffer[BUFFER_SIZE];//almacena los datos mientas duermr
    RTC_DATA_ATTR int index;//guarda el sigiente dato
    RTC_DATA_ATTR int count;// cuenta los datos guardados
    int index;
    int count;

public:
    DataBuffer();// agrea nueva nedicion
    void add(const SensorData &data);
    SensorData get(int position);// obtiene los datos almacenados
    int size();
};
#endif