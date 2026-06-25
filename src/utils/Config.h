#ifndef CONFIG_H
#define CONFIG_H

// ── Identificación del nodo ───────────────────────────────────
#define DEVICE_ID  "ESTACION_01"
#define API_KEY    "ULEAM_2026"

// ── Tiempos y muestreo ────────────────────────────────────────
#define WAKE_INTERVAL              60
#define SENSOR_STABILIZATION_TIME  2000
#define SAMPLE_COUNT               5
#define SAMPLE_INTERVAL            500
#define MEASURE_WINDOW_MIN         2

// ── Umbrales de anomalías ─────────────────────────────────────
#define ANOMALY_TEMP_DELTA  2.0f
#define ANOMALY_HUM_DELTA   5.0f

// ── Rangos de validación del sensor ──────────────────────────
#define TEMP_MIN   15.0f
#define TEMP_MAX   45.0f
#define HUM_MIN     0.0f
#define HUM_MAX   100.0f

// ── Sensor DHT22 ──────────────────────────────────────────────
#define DHT_PIN   23
#define DHT_TYPE  DHT22

// ── Comunicación LoRa (UART) ──────────────────────────────────
#define LORA_RX  16
#define LORA_TX  17

#endif