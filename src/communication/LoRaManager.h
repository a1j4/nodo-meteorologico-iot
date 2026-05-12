#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include <Arduino.h>
#include <HardwareSerial.h>

class LoRaManager {

private:
    HardwareSerial* serial;

public:
    LoRaManager(HardwareSerial* serialPort);

    void begin();
    void sendCmd(String cmd);
    void sendPayload(String payload);
};

#endif