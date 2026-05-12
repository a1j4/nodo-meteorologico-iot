#include "DataBuffer.h"

// 1. Definición de las variables globales en memoria RTC (solo se inicializan una vez)
RTC_DATA_ATTR SensorData rtc_buffer[BUFFER_SIZE];
RTC_DATA_ATTR int rtc_index = 0;
RTC_DATA_ATTR int rtc_count = 0;

// Implementación de la memoria circular
DataBuffer::DataBuffer() {
    // No reseteamos index ni count aquí porque borraríamos los datos al despertar del Deep Sleep
}

// Guarda los datos en la posición actual usando la memoria RTC
void DataBuffer::add(const SensorData& data) {
    rtc_buffer[rtc_index] = data;
    rtc_index = (rtc_index + 1) % BUFFER_SIZE;
    if (rtc_count < BUFFER_SIZE) {
        rtc_count++;
    }
}

// Permite obtener un dato y verificar si existe
SensorData DataBuffer::get(int position) {

    if (position < 0 || position >= rtc_count) {

        SensorData empty;

        empty.valid = false;

        return empty;
    }

    // Calcular posición real en buffer circular
    int realIndex =
        (rtc_index - rtc_count + position + BUFFER_SIZE)
        % BUFFER_SIZE;

    return rtc_buffer[realIndex];
}

// Devuelve cuántos datos hay guardados 
int DataBuffer::size() {
    return rtc_count;
}
