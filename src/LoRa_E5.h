#ifndef LORA_E5_H
#define LORA_E5_H

#include <Arduino.h>

class LoRa_E5 {
public:
    LoRa_E5(HardwareSerial &serial, uint32_t baud, int8_t rxPin = -1, int8_t txPin = -1);

    bool begin(unsigned long timeout = 1000);
    bool initializeModule();
    bool joinNetwork(const String &devEUI, const String &appEUI, const String &appKey, unsigned long timeout = 30000);
    bool sendMessage(const String &data, uint8_t port = 1, bool confirmed = false);

    // New methods
    bool setDataRate(uint8_t dataRate);
    bool setTxPower(uint8_t txPower);
    String getDeviceStatus();
    float getBatteryVoltage();
    int16_t getRSSI();
    int8_t getSNR();

private:
    HardwareSerial &_serial;
    uint32_t _baud;
    int8_t _rxPin;
    int8_t _txPin;

    bool sendATCommand(const String &command, const String &expectedResponse, unsigned long timeout = 1000);
    String readResponse(unsigned long timeout = 1000);
    void flushSerial();
};

#endif // LORA_E5_H
