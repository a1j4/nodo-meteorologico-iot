#include "LoRaManager.h"

#define RXD2 16
#define TXD2 17

LoRaManager::LoRaManager(HardwareSerial* serialPort) {
    serial = serialPort;
}

void LoRaManager::begin() {

    serial->begin(115200, SERIAL_8N1, RXD2, TXD2);

    delay(1000);

    sendCmd("AT+ADDRESS=1");
    sendCmd("AT+NETWORKID=5");
}

void LoRaManager::sendCmd(String cmd) {

    serial->println(cmd);

    delay(1000);

    while (serial->available()) {

        String res = serial->readStringUntil('\n');

        res.trim();

        if (res.length() > 0) {
            Serial.println("[LoRa]: " + res);
        }
    }
}

void LoRaManager::sendPayload(String payload) {

    String datalen = String(payload.length());

    sendCmd("AT+SEND=2," + datalen + "," + payload);
}