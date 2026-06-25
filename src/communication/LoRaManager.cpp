#include "LoRaManager.h"
#include "../utils/Config.h"

LoRaManager::LoRaManager(HardwareSerial* serialPort) {
    serial = serialPort;
}

void LoRaManager::begin() {
    serial->begin(115200, SERIAL_8N1, LORA_RX, LORA_TX);
    delay(1000);
    sendCmd("AT+ADDRESS=1");
    sendCmd("AT+NETWORKID=5");
}

// Envía un comando AT y espera respuesta con timeout.
// Retorna true si el módulo responde "+OK".
bool LoRaManager::sendCmd(const String& cmd) {
    serial->println(cmd);

    unsigned long start = millis();
    while (millis() - start < LORA_CMD_TIMEOUT) {
        if (serial->available()) {
            String res = serial->readStringUntil('\n');
            res.trim();
            if (res.length() > 0) {
                Serial.println("[LoRa] " + res);
                if (res == "+OK") return true;
            }
        }
    }
    Serial.println("[LoRa] Sin respuesta para: " + cmd);
    return false;
}

// Transmite el payload por LoRa al nodo destino (ADDRESS=2).
// Retorna false si el payload supera el límite o el módulo no confirma.
bool LoRaManager::sendPayload(const String& payload) {
    if (payload.length() > LORA_MAX_PAYLOAD) {
        Serial.printf("[LoRa] Payload demasiado largo (%d bytes, máx %d)\n",
                      payload.length(), LORA_MAX_PAYLOAD);
        return false;
    }
    String cmd = "AT+SEND=2," + String(payload.length()) + "," + payload;
    return sendCmd(cmd);
}