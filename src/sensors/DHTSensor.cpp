#include "DHTSensor.h"
#include <Arduino.h>

DHTSensor::DHTSensor() : _dht(DHT_PIN, DHT_TYPE) {}

void DHTSensor::begin() {
    _dht.begin();
}

// Retorna una lectura del DHT22. El timestamp lo asigna sampleAndFilter()
// usando el RTC, por eso aquí se deja en 0.
SensorData DHTSensor::read() {
    SensorData data;
    data.temperature = _dht.readTemperature();
    data.humidity    = _dht.readHumidity();
    data.timestamp   = 0;
    data.valid       = !isnan(data.temperature) && !isnan(data.humidity);
    return data;
}
