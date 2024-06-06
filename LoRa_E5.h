#ifndef LORA_E5_H
#define LORA_E5_H

#include <Arduino.h>

class LoRa_E5 {
public:
    LoRa_E5(HardwareSerial &serial, uint32_t baud, int8_t rxPin, int8_t txPin);
    void begin();
    bool sendATCommand(const String& command);
    String readResponse(uint16_t timeout = 1000);
    bool initializeModule();
    bool joinNetwork(const String& devEUI, const String& appEUI, const String& appKey);
    bool sendMessage(const String& data);

private:
    HardwareSerial &_serial;
    uint32_t _baud;
    int8_t _rxPin;
    int8_t _txPin;
    bool checkResponse(const String& expected);
};

#endif
