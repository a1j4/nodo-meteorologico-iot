#ifndef CONFIG_H
#define CONFIG_H

// ==========================
// IDENTIFICACIÓN DEL NODO
// ==========================

#define DEVICE_ID "ESTACION_01"
#define API_KEY "ULEAM_2026"

// ==========================
// CONFIGURACIÓN DE TIEMPOS
// ==========================

#define SENSOR_READ_INTERVAL 2000
#define WAKE_INTERVAL 60

#define SENSOR_STABILIZATION_TIME 2000
#define SAMPLE_COUNT 5
#define SAMPLE_INTERVAL 500

// ==========================
// CONFIGURACIÓN DEL DHT22
// ==========================

#define DHT_PIN 23
#define DHT_TYPE DHT22

// ==========================
// CONFIGURACIÓN LORA
// ==========================

#define LORA_RX 16
#define LORA_TX 17

#endif
