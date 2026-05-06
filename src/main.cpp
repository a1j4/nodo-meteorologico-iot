#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <esp_sleep.h>//control del modo deep slepp

#include "utils/Config.h"
#include "processing/DataProcessor.h"
#include "utils/SensorData.h"
#include "sensors/SensorManager.h"
#include "storage/DataBuffer.h"


DataBuffer buffer;// almacenan varias mediciones



void goToSleep();//funcion de estado deep sleep

// controla la lectura de sensores
SensorManager sensorManager;

// ===== RTC =====
RTC_DS3231 rtc; //obj reloj
RTC_DATA_ATTR int lastMeasuredHour = -1;//guarda la hora ultima medicion
RTC_DATA_ATTR SensorData lastMeasurement;//ultima medicion
RTC_DATA_ATTR bool hasLastMeasurement = false;// encarga de indicar si existen datos previos

// ===== detecta las anomalias =====
bool detectAnomaly(const SensorData& current, const SensorData& previous) {
    if (abs(current.temperature - previous.temperature) > 2.0) return true;
    if (abs(current.humidity - previous.humidity) > 5.0) return true;
    if (abs(current.pressure - previous.pressure) > 4.0) return true;
    return false;
}
//calcula  la tendecias
String calculateTrend(const SensorData& current, const SensorData& previous) {

    if (!hasLastMeasurement) {
        return "sin_datos";
    }

    float deltaTemp = current.temperature - previous.temperature;

    if (deltaTemp > 0.5) {
        return "subiendo";
    } else if (deltaTemp < -0.5) {
        return "bajando";
    } else {
        return "estable";
    }
}
// encarga de filtrar datos
SensorData sampleAndFilter() {

    SensorData samples[SAMPLE_COUNT];//arrelogo de muestras

    // 1️⃣ Estabilización del sensor
    delay(SENSOR_STABILIZATION_TIME);


    // 2️⃣ Tomar múltiples muestras
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        samples[i] = sensorManager.readSensors();
        delay(SAMPLE_INTERVAL);
    }

    // 3️⃣ Inicializar min y max
    float minTemp = samples[0].temperature;
    float maxTemp = samples[0].temperature;
    float minHum  = samples[0].humidity;
    float maxHum  = samples[0].humidity;
    float minPres = samples[0].pressure;
    float maxPres = samples[0].pressure;

    float sumTemp = 0;
    float sumHum  = 0;
    float sumPres = 0;

    // 4️⃣ Encontrar extremos y sumar
    for (int i = 0; i < SAMPLE_COUNT; i++) {

        float t = samples[i].temperature;
        float h = samples[i].humidity;
        float p = samples[i].pressure;

        minTemp = min(minTemp, t);
        maxTemp = max(maxTemp, t);
        minHum  = min(minHum, h);
        maxHum  = max(maxHum, h);
        minPres = min(minPres, p);
        maxPres = max(maxPres, p);

        sumTemp += t;
        sumHum  += h;
        sumPres += p;
    }

    // 5️⃣ Eliminar extremos (min + max)
    SensorData filtered;

    filtered.temperature =
        (sumTemp - minTemp - maxTemp) / (SAMPLE_COUNT - 2);
    filtered.humidity =
        (sumHum - minHum - maxHum) / (SAMPLE_COUNT - 2);
    filtered.pressure =
        (sumPres - minPres - maxPres) / (SAMPLE_COUNT - 2);

    filtered.timestamp = millis();
    filtered.valid = true;

    return filtered;
}
//analiza la detecion de tecnicas generales compara datos de la anterior medida  ejemplo: 25C, 22.5C, 25.5C la diferensia es 0.5
void analyzeTrend(){
     int size = buffer.size();

    if (size < 3) {
        Serial.println("No hay suficientes datos para tendencia");
        return;
    }

    SensorData first = buffer.get(0);//dato más atiguo
    SensorData last  = buffer.get(size - 1);//EL ULTIMO DATO

    float tempDiff = last.temperature - first.temperature;
    float humDiff  = last.humidity - first.humidity;

    // Temperatura
    if (tempDiff > 1.0) {
        Serial.println(" Temperatura en aumento");
    } else if (tempDiff < -1.0) {
        Serial.println(" Temperatura en descenso");
    } else {
        Serial.println(" Temperatura estable");
    }

    // Humedad
    if (humDiff > 3.0) {
        Serial.println(" Humedad en aumento");
    } else if (humDiff < -3.0) {
        Serial.println("Humedad en descenso");
    } else {
        Serial.println("Humedad estable");
    }
}
//sirve para  detectar si algien maipulo manualmente  los datos 
String generateSignature(String payload) {
    int hash = 0;

    for (int i = 0; i < payload.length(); i++) {
        hash += payload[i] * (i + 1);
    }

    return String(hash);
}
//se encarga de generar el dato JSON
String preparePayload(const SensorData& data, bool anomaly, String trend) {
    DateTime now = rtc.now();
    String payload = "{";

    payload += "\"temp\":" + String(data.temperature, 2) + ",";
    payload += "\"hum\":" + String(data.humidity, 2) + ",";
    payload += "\"pres\":" + String(data.pressure, 2) + ",";
    payload += "\"hora\":\"" + String(now.hour()) + ":" + String(now.minute()) + "\",";
    payload += "\"fecha\":\"" + String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + "\",";
    payload += "\"trend\":\"" + trend + "\",";
    payload += "\"anomaly\":" + String(anomaly ? "true" : "false");

    payload += "}";
    // 🔐 Generar firma
    String signature = generateSignature(payload);

    // Agregar firma al final
    payload = payload.substring(0, payload.length() - 1);
    payload += ",\"sig\":\"" + signature + "\"}";


    return payload;
}

void setup() {

    Serial.begin(115200);//inicia la comunicacion
    Wire.begin();

    if (!rtc.begin()) {//detecta si el relod funciona, se detiene si no funciona
        Serial.println("No se encontró el RTC");
        while (1);
    }

    Serial.println("Despertando estación...");

    // Obtener hora actual
    DateTime now = rtc.now();
    int hora = now.hour();
    int minuto = now.minute();

    Serial.print("Hora actual: ");
    Serial.print(hora);
    Serial.print(":");
    Serial.println(minuto);

    bool shouldMeasure = false;

    // Solo medir a horas específicas
    if ((hora == 6 || hora == 12 || hora == 18) && (minuto >= 0 && minuto <= 2) && hora != lastMeasuredHour) {
        shouldMeasure = true;
        lastMeasuredHour = hora;
    }

    if (!shouldMeasure) {
        Serial.println("No es hora de medir");
        goToSleep();//controla la hora de medición
    }

    Serial.println("Realizando medición programada...");

    sensorManager.begin();// realiza las mediciones
    SensorData currentData = sampleAndFilter();

    DataProcessor processor;
    currentData = processor.validate(currentData);// verifica que los datos sean correctos
    if(currentData.valid){//verifica si ahy datos anomalos
        buffer.add(currentData);
        analyzeTrend();
    }

    if (!currentData.valid) {
        Serial.println("Error: sensor no válido");
        goToSleep();
    }

    bool anomalyDetected = false;
    String trend = calculateTrend(currentData, lastMeasurement);//compara datos anomalos anteriores

    if (hasLastMeasurement) {
        anomalyDetected = detectAnomaly(currentData, lastMeasurement);
    }

    if (anomalyDetected) {
        Serial.println(" Anomalía detectada");
    } else {
        Serial.println("Medición estable");
    }

    

    String payload = preparePayload(currentData, anomalyDetected,trend);//prepara el dato JSON
    Serial.println(" Datos listos para enviar:");
    Serial.println(payload);
   

    lastMeasurement = currentData;// guarda la medicion actual
    hasLastMeasurement = true;

    goToSleep();
}


//controla el estado del ESP32 
void goToSleep() {
    Serial.println("Entrando en Deep Sleep...");
    esp_sleep_enable_timer_wakeup(60*1000000ULL);
    esp_deep_sleep_start();
}

void loop(){
    
}
