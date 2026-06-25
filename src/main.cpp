#include <Arduino.h>
#include <DHT.h>
#include <RTClib.h>
#include <Wire.h>
#include <esp_sleep.h>

#include "utils/Config.h"
#include "utils/SensorData.h"
#include "sensors/SensorManager.h"
#include "processing/DataProcessor.h"
#include "storage/DataBuffer.h"
#include "communication/LoRaManager.h"
#include "security/SecurityManager.h"

// SAMPLE_COUNT debe ser > 2 para la media recortada (Config.h)
static_assert(SAMPLE_COUNT > 2, "SAMPLE_COUNT debe ser mayor a 2.");

// ── Objetos globales ──────────────────────────────────────────
static SensorManager   sensorManager;
static DataProcessor   processor;
static DataBuffer      buffer;
static LoRaManager     loraManager(&Serial2);
static SecurityManager securityManager;
static RTC_DS3231      rtc;

// ── Estado persistente en deep sleep ─────────────────────────
RTC_DATA_ATTR static int        lastMeasuredHour = -1;
RTC_DATA_ATTR static int        lastMeasuredDay  = -1;
RTC_DATA_ATTR static SensorData lastMeasurement;
RTC_DATA_ATTR static bool       hasLastMeasurement = false;

// ─────────────────────────────────────────────────────────────

// Único punto de salida del firmware
static void goToSleep() {
    Serial.println("Entrando en deep sleep (60 s)...");
    Serial.flush();
    esp_sleep_enable_timer_wakeup(60ULL * 1'000'000ULL);
    esp_deep_sleep_start();
}

// Toma SAMPLE_COUNT muestras y aplica media recortada (descarta min y max)
static SensorData sampleAndFilter(const DateTime& rtcNow) {
    delay(SENSOR_STABILIZATION_TIME);

    SensorData samples[SAMPLE_COUNT];
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        samples[i] = sensorManager.readSensors();
        delay(SAMPLE_INTERVAL);
    }

    float minTemp = samples[0].temperature, maxTemp = samples[0].temperature;
    float minHum  = samples[0].humidity,    maxHum  = samples[0].humidity;
    float sumTemp = 0.0f, sumHum = 0.0f;

    for (int i = 0; i < SAMPLE_COUNT; i++) {
        float t = samples[i].temperature;
        float h = samples[i].humidity;
        if (t < minTemp) minTemp = t;
        if (t > maxTemp) maxTemp = t;
        if (h < minHum)  minHum  = h;
        if (h > maxHum)  maxHum  = h;
        sumTemp += t;
        sumHum  += h;
    }

    SensorData filtered;
    filtered.temperature = (sumTemp - minTemp - maxTemp) / (SAMPLE_COUNT - 2);
    filtered.humidity    = (sumHum  - minHum  - maxHum)  / (SAMPLE_COUNT - 2);
    filtered.timestamp   = rtcNow.unixtime();
    filtered.valid       = isfinite(filtered.temperature) && isfinite(filtered.humidity);
    return filtered;
}

// Retorna "subiendo", "bajando" o "estable" comparando con la medición anterior
static String calculateTrend(const SensorData& current, const SensorData& previous) {
    if (!hasLastMeasurement) return "sin_datos";
    float delta = current.temperature - previous.temperature;
    if (delta >  0.5f) return "subiendo";
    if (delta < -0.5f) return "bajando";
    return "estable";
}

// Imprime tendencia de temperatura y humedad usando el buffer acumulado
static void analyzeTrend() {
    int size = buffer.size();
    if (size < 3) {
        Serial.println("Buffer insuficiente para tendencia.");
        return;
    }

    float dTemp = buffer.get(size - 1).temperature - buffer.get(0).temperature;
    float dHum  = buffer.get(size - 1).humidity    - buffer.get(0).humidity;

    if      (dTemp >  1.0f) Serial.println("[Tendencia] Temperatura en aumento.");
    else if (dTemp < -1.0f) Serial.println("[Tendencia] Temperatura en descenso.");
    else                    Serial.println("[Tendencia] Temperatura estable.");

    if      (dHum >  3.0f) Serial.println("[Tendencia] Humedad en aumento.");
    else if (dHum < -3.0f) Serial.println("[Tendencia] Humedad en descenso.");
    else                   Serial.println("[Tendencia] Humedad estable.");
}

// Retorna true si temperatura o humedad superan los umbrales definidos en Config.h
static bool detectAnomaly(const SensorData& current, const SensorData& previous) {
    return fabsf(current.temperature - previous.temperature) > ANOMALY_TEMP_DELTA ||
           fabsf(current.humidity    - previous.humidity)    > ANOMALY_HUM_DELTA;
}

// Construye el JSON firmado listo para transmitir por LoRa
static String preparePayload(const SensorData& data, bool anomaly,
                             const String& trend, const DateTime& now) {
    char horaStr[6],  fechaStr[11], base[256], full[320];

    snprintf(horaStr,  sizeof(horaStr),  "%02d:%02d", now.hour(), now.minute());
    snprintf(fechaStr, sizeof(fechaStr), "%02d/%02d/%04d", now.day(), now.month(), now.year());
    snprintf(base, sizeof(base),
             "{\"temp\":%.2f,\"hum\":%.2f,\"hora\":\"%s\","
             "\"fecha\":\"%s\",\"trend\":\"%s\",\"anomaly\":%s}",
             data.temperature, data.humidity, horaStr, fechaStr,
             trend.c_str(), anomaly ? "true" : "false");

    String sig = securityManager.generateSignature(String(base));
    snprintf(full, sizeof(full), "%.*s,\"sig\":\"%s\"}",
             (int)(strlen(base) - 1), base, sig.c_str());

    return String(full);
}

// Ciclo completo: verificar ventana → medir → analizar → transmitir → dormir
static void runCycle() {
    DateTime now   = rtc.now();
    int hora       = now.hour();
    int minuto     = now.minute();
    int dia        = now.day();

    bool enVentana   = (minuto >= 0 && minuto <= MEASURE_WINDOW_MIN) &&
                       (hora == 6 || hora == 12 || hora == 18);
    bool yaRealizado = (hora == lastMeasuredHour && dia == lastMeasuredDay);

    if (!enVentana || yaRealizado) {
        Serial.println("Fuera de ventana. Durmiendo.");
        goToSleep();
    }

    Serial.printf("Medición – %02d:%02d del %02d/%02d/%04d\n",
                  hora, minuto, dia, now.month(), now.year());

    SensorData current = processor.validate(sampleAndFilter(now));
    if (!current.valid) {
        Serial.println("[Error] Sensor no válido. Abortando.");
        goToSleep();
    }

    buffer.add(current);
    analyzeTrend();

    bool   anomaly = false;
    String trend   = "sin_datos";
    if (hasLastMeasurement) {
        trend   = calculateTrend(current, lastMeasurement);
        anomaly = detectAnomaly(current, lastMeasurement);
    }

    Serial.printf("[Estado] Tendencia: %s | Anomalía: %s\n",
                  trend.c_str(), anomaly ? "SÍ" : "no");

    String payload = preparePayload(current, anomaly, trend, now);
    Serial.println("[JSON] " + payload);
    loraManager.sendPayload(payload);

    lastMeasurement    = current;
    hasLastMeasurement = true;
    lastMeasuredHour   = hora;
    lastMeasuredDay    = dia;

    Serial.println("Ciclo completado.");
    goToSleep();
}

// ─────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    Wire.begin();
    delay(100);

    Serial.println("=== ESTACIÓN METEOROLÓGICA ===");

    if (!rtc.begin()) {
        Serial.println("[Error crítico] RTC no encontrado.");
        goToSleep();
    }

    if (rtc.lostPower())
        Serial.println("[Advertencia] RTC perdió alimentación. Verificar hora.");

    sensorManager.begin();
    loraManager.begin();

    runCycle();
}

void loop() {}