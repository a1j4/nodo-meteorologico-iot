#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <esp_sleep.h>
#include <DHT.h>

#include "utils/Config.h"
#include "utils/SensorData.h"

#include "sensors/SensorManager.h"

#include "processing/DataProcessor.h"

#include "storage/DataBuffer.h"

#include "communication/LoRaManager.h"

#include "security/SecurityManager.h"

// ======================================================
// ================= OBJETOS GLOBALES ===================
// ======================================================

SensorManager sensorManager;

DataProcessor processor;

DataBuffer buffer;

LoRaManager loraManager(&Serial2);

SecurityManager securityManager;

// ======================================================
// ==================== RTC =============================
// ======================================================

RTC_DS3231 rtc;

RTC_DATA_ATTR int lastMeasuredHour = -1;

RTC_DATA_ATTR SensorData lastMeasurement;

RTC_DATA_ATTR bool hasLastMeasurement = false;

// ======================================================
// ============== DETECCIÓN DE ANOMALÍAS ===============
// ======================================================

bool detectAnomaly(const SensorData& current,
                   const SensorData& previous) {

    if (abs(current.temperature - previous.temperature) > 2.0)
        return true;

    if (abs(current.humidity - previous.humidity) > 5.0)
        return true;

    if (abs(current.pressure - previous.pressure) > 4.0)
        return true;

    return false;
}

// ======================================================
// ================= CÁLCULO DE TENDENCIA ==============
// ======================================================

String calculateTrend(const SensorData& current,
                      const SensorData& previous) {

    if (!hasLastMeasurement) {
        return "sin_datos";
    }

    float deltaTemp =
        current.temperature - previous.temperature;

    if (deltaTemp > 0.5) {

        return "subiendo";

    } else if (deltaTemp < -0.5) {

        return "bajando";

    } else {

        return "estable";
    }
}

// ======================================================
// ================= FILTRADO DE DATOS ==================
// ======================================================

SensorData sampleAndFilter() {

    SensorData samples[SAMPLE_COUNT];

    // Estabilización
    delay(SENSOR_STABILIZATION_TIME);

    // Toma de muestras
    for (int i = 0; i < SAMPLE_COUNT; i++) {

        samples[i] = sensorManager.readSensors();

        delay(SAMPLE_INTERVAL);
    }

    // Variables iniciales
    float minTemp = samples[0].temperature;
    float maxTemp = samples[0].temperature;

    float minHum = samples[0].humidity;
    float maxHum = samples[0].humidity;

    float minPres = samples[0].pressure;
    float maxPres = samples[0].pressure;

    float sumTemp = 0;
    float sumHum = 0;
    float sumPres = 0;

    // Recorrido de muestras
    for (int i = 0; i < SAMPLE_COUNT; i++) {

        float t = samples[i].temperature;
        float h = samples[i].humidity;
        float p = samples[i].pressure;

        minTemp = min(minTemp, t);
        maxTemp = max(maxTemp, t);

        minHum = min(minHum, h);
        maxHum = max(maxHum, h);

        minPres = min(minPres, p);
        maxPres = max(maxPres, p);

        sumTemp += t;
        sumHum += h;
        sumPres += p;
    }

    // Media recortada
    SensorData filtered;

    filtered.temperature =
        (sumTemp - minTemp - maxTemp) /
        (SAMPLE_COUNT - 2);

    filtered.humidity =
        (sumHum - minHum - maxHum) /
        (SAMPLE_COUNT - 2);

    filtered.pressure =
        (sumPres - minPres - maxPres) /
        (SAMPLE_COUNT - 2);

    filtered.timestamp = millis();

    filtered.valid = true;

    return filtered;
}

// ======================================================
// ================= ANÁLISIS DE TENDENCIA ==============
// ======================================================

void analyzeTrend() {

    int size = buffer.size();

    if (size < 3) {

        Serial.println(
            "No hay suficientes datos para tendencia");

        return;
    }

    SensorData first = buffer.get(0);

    SensorData last = buffer.get(size - 1);

    float tempDiff =
        last.temperature - first.temperature;

    float humDiff =
        last.humidity - first.humidity;

    // Temperatura
    if (tempDiff > 1.0) {

        Serial.println("Temperatura en aumento");

    } else if (tempDiff < -1.0) {

        Serial.println("Temperatura en descenso");

    } else {

        Serial.println("Temperatura estable");
    }

    // Humedad
    if (humDiff > 3.0) {

        Serial.println("Humedad en aumento");

    } else if (humDiff < -3.0) {

        Serial.println("Humedad en descenso");

    } else {

        Serial.println("Humedad estable");
    }
}

// ======================================================
// ================= GENERACIÓN JSON ====================
// ======================================================

String preparePayload(const SensorData& data,
                      bool anomaly,
                      String trend) {

    // Fecha simulada temporalmente
    int hora = 6;
    int minuto = 0;

    int dia = 1;
    int mes = 1;
    int anio = 2025;

    String payload = "{";

    payload += "\"temp\":" +
               String(data.temperature, 2) + ",";

    payload += "\"hum\":" +
               String(data.humidity, 2) + ",";

    payload += "\"pres\":" +
               String(data.pressure, 2) + ",";

    payload += "\"hora\":\"" +
               String(hora) + ":" +
               String(minuto) + "\",";

    payload += "\"fecha\":\"" +
               String(dia) + "/" +
               String(mes) + "/" +
               String(anio) + "\",";

    payload += "\"trend\":\"" +
               trend + "\",";

    payload += "\"anomaly\":" +
               String(anomaly ? "true" : "false");

    payload += "}";

    // Firma de seguridad
    String signature =
        securityManager.generateSignature(payload);

    // Agregar firma
    payload =
        payload.substring(0, payload.length() - 1);

    payload += ",\"sig\":\"" +
               signature + "\"}";

    return payload;
}

// ======================================================
// ===================== DEEP SLEEP =====================
// ======================================================

void goToSleep() {

    Serial.println("Entrando en Deep Sleep...");

    esp_sleep_enable_timer_wakeup(
        60 * 1000000ULL);

    esp_deep_sleep_start();
}

// ======================================================
// ======================== SETUP =======================
// ======================================================

void setup() {

    // Comunicaciones
    Serial.begin(115200);

    Wire.begin();

    delay(2000);

    Serial.println(
        "=== INICIANDO ESTACIÓN METEOROLÓGICA ===");

    // ==================================================
    // =============== INICIALIZACIÓN LORA ==============
    // ======= SOLO MODIFICAR ESTA SECCIÓN ==============
    // ==================================================

    loraManager.begin();

    // ==================================================
    // ============= VALIDACIÓN DE MEDICIÓN =============
    // ==================================================

    // Simulación temporal sin RTC
    int hora = 6;

    int minuto = 1;

    bool shouldMeasure = false;

    if ((hora == 6 ||
         hora == 12 ||
         hora == 18) &&

        (minuto >= 0 &&
         minuto <= 2) &&

        hora != lastMeasuredHour) {

        shouldMeasure = true;

        lastMeasuredHour = hora;
    }

    if (!shouldMeasure) {

        Serial.println("No es hora de medir");

        goToSleep();
    }

    // ==================================================
    // ================= LECTURA DE DATOS ===============
    // ==================================================

    Serial.println(
        "Realizando medición programada...");

    sensorManager.begin();

    SensorData currentData =
        sampleAndFilter();

    currentData =
        processor.validate(currentData);

    if (currentData.valid) {

        buffer.add(currentData);

        analyzeTrend();

    } else {

        Serial.println(
            "Error: sensor no válido");

        goToSleep();
    }

    // ==================================================
    // ========= DETECCIÓN DE ANOMALÍAS =================
    // ==================================================

    bool anomalyDetected = false;

    String trend = "estable";

    if (hasLastMeasurement) {

        trend =
            calculateTrend(currentData,
                            lastMeasurement);

        anomalyDetected =
            detectAnomaly(currentData,
                          lastMeasurement);
    }

    if (anomalyDetected) {

        Serial.println(
            "⚠ Anomalía detectada");

    } else {

        Serial.println(
            "Medición estable");
    }

    // ==================================================
    // ================ GENERACIÓN JSON =================
    // ==================================================

    String payload =
        preparePayload(currentData,
                       anomalyDetected,
                       trend);

    Serial.println(
        "JSON generado:");

    Serial.println(payload);

    // ==================================================
    // ============= TRANSMISIÓN LoRa ===================
    // ===== SOLO MODIFICAR ESTA SECCIÓN ================
    // ==================================================

    Serial.println(
        "Transmitiendo datos por LoRa...");

    loraManager.sendPayload(payload);

    // ==================================================
    // ================ GUARDAR ESTADO ==================
    // ==================================================

    lastMeasurement = currentData;

    hasLastMeasurement = true;

    Serial.println(
        "Proceso completado correctamente");

    // Dormir
    goToSleep();
}

// ======================================================
// ========================= LOOP =======================
// ======================================================

void loop() {

}