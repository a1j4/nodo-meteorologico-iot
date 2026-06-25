#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include <Arduino.h>
#include <HardwareSerial.h>

#define LORA_MAX_PAYLOAD  240   // Límite de bytes por trama del módulo RYLR
#define LORA_CMD_TIMEOUT  2000  // Timeout de espera de respuesta en ms

class LoRaManager {
private:
    HardwareSerial* serial;

    // Envía un comando AT y retorna true si el módulo responde "+OK"
    bool sendCmd(const String& cmd);

public:
    LoRaManager(HardwareSerial* serialPort);
    void begin();
    bool sendPayload(const String& payload);  // Retorna true si la transmisión fue aceptada
};

#endif