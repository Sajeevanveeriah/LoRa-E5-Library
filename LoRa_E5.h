#ifndef LORA_E5_H
#define LORA_E5_H

#include <Arduino.h>

class LoRa_E5 {
public:
    LoRa_E5(HardwareSerial &serial, uint32_t baud);
    void begin();
    void sendATCommand(String command);
    String readResponse();
    void initializeModule();
    void joinNetwork(String devEUI, String appEUI, String appKey);
    void sendMessage(String data);

private:
    HardwareSerial &_serial;
    uint32_t _baud;
};

#endif
