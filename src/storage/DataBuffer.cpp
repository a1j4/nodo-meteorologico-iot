#include "DataBuffer.h"

// Definición de las variables globales en memoria RTC
// Solo se inicializan en el primer arranque; el deep sleep las preserva
RTC_DATA_ATTR SensorData rtc_buffer[BUFFER_SIZE];
RTC_DATA_ATTR int rtc_index = 0;
RTC_DATA_ATTR int rtc_count = 0;

// Guarda un dato en la posición actual y avanza el índice circularmente
void DataBuffer::add(const SensorData& data) {
    rtc_buffer[rtc_index] = data;
    rtc_index = (rtc_index + 1) % BUFFER_SIZE;
    if (rtc_count < BUFFER_SIZE) rtc_count++;
}

// Retorna el dato en la posición lógica indicada (0 = más antiguo, size-1 = más reciente)
// Si la posición es inválida, retorna un SensorData vacío con valid = false
SensorData DataBuffer::get(int position) const {
    if (position < 0 || position >= rtc_count) {
        SensorData empty = {};   // Inicializa todos los campos a cero
        empty.valid = false;
        return empty;
    }
    int realIndex = (rtc_index - rtc_count + position + BUFFER_SIZE) % BUFFER_SIZE;
    return rtc_buffer[realIndex];
}

// Retorna cuántos datos hay almacenados actualmente
int DataBuffer::size() const {
    return rtc_count;
}