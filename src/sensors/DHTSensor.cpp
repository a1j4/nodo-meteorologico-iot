#include "DHTSensor.h"
#include <Arduino.h>

// Constructor
DHTSensor::DHTSensor()
    : _dht(DHT_PIN, DHT_TYPE) {
}

// Inicialización del sensor
void DHTSensor::begin() {

    _dht.begin();
}

// Lectura del sensor
SensorData DHTSensor::read() {

    SensorData data;

    // Lecturas reales del DHT22
    data.humidity = _dht.readHumidity();
    data.temperature = _dht.readTemperature();
 
    // Timestamp
    data.timestamp = millis();

    // Validación
    data.valid =
        !isnan(data.humidity) &&
        !isnan(data.temperature);

    return data;
}
