# 🌦 Nodo Meteorológico IoT con ESP32

Sistema embebido para monitoreo climático autónomo utilizando ESP32, diseñado para funcionar en zonas rurales sin acceso a energía eléctrica continua.

---

## 🚀 Características

* Medición de:

  * 🌡 Temperatura
  * 💧 Humedad
  * 🌬 Presión atmosférica
* 🧠 Procesamiento en el borde (Edge Computing)
* 📊 Filtrado de datos (media recortada)
* ⚠ Detección de anomalías
* ⏰ Medición programada (06:00, 12:00, 18:00)
* 🔋 Modo Deep Sleep para ahorro de energía
* 📦 Generación de datos en formato JSON
* 📡 Preparado para integración con LoRa

---

## 🧱 Arquitectura del Proyecto

```
src/
├── main.cpp
├── sensors/
│   ├── DHTSensor.h
│   ├── DHTSensor.cpp
│   ├── SensorManager.h
│   └── SensorManager.cpp
├── processing/
│   ├── DataProcessor.h
│   └── DataProcessor.cpp
├── storage/
│   ├── DataBuffer.h
│   └── DataBuffer.cpp
├── utils/
│   ├── SensorData.h
│   └── Config.h
```

---

## ⚙️ Requisitos

### Hardware

* ESP32 (NodeMCU-32S recomendado)
* Sensor DHT22 (o similar)
* RTC DS3231
* (Opcional) Módulo LoRa (SX1276 / SX1278)

---

## 🛠️ Configuración

Editar el archivo:

```
src/utils/Config.h
```

Ejemplo:

```cpp
#define DHT_PIN 8
#define SENSOR_STABILIZATION_TIME 2000
#define SAMPLE_COUNT 5
#define SAMPLE_INTERVAL 500
```

---

## ▶️ Cómo usar el proyecto

1. Abrir el proyecto en PlatformIO
2. Conectar el ESP32
3. Compilar el proyecto
4. Subir al ESP32
5. Abrir monitor serial

---

## 🔄 Funcionamiento del sistema

1. El ESP32 se despierta
2. Verifica la hora usando RTC
3. Solo mide a las:

   * 06:00
   * 12:00
   * 18:00
4. Estabiliza el sensor
5. Toma múltiples muestras
6. Filtra datos (elimina valores extremos)
7. Valida datos
8. Detecta anomalías
9. Genera payload
10. Entra en Deep Sleep

---

## 📦 Ejemplo de datos generados (Payload)

```json
{
  "temp": 25.3,
  "hum": 60.2,
  "pres": 1012.5,
  "time": 12345678,
  "anomaly": false
}
```

---

## 📡 Integración con LoRa

El sistema ya deja los datos listos para enviar.

Tu compañero solo debe agregar:

```cpp
LoRa.beginPacket();
LoRa.print(payload);
LoRa.endPacket();
```

---

## 🔋 Consumo energético

Optimizado para:

* Batería Li-ion
* Panel solar
* Uso en campo sin electricidad

---

## 📈 Mejoras futuras

* Integración con LoRaWAN
* Aplicación móvil
* Recomendaciones agrícolas inteligentes
* Soporte para múltiples sensores

---

## 👨‍💻 Autor

Proyecto de titulación – Ingeniería de Software


