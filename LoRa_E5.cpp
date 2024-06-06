#include "LoRa_E5.h"

LoRa_E5::LoRa_E5(HardwareSerial &serial, uint32_t baud, int8_t rxPin, int8_t txPin)
    : _serial(serial), _baud(baud), _rxPin(rxPin), _txPin(txPin) {}

void LoRa_E5::begin() {
    _serial.begin(_baud, SERIAL_8N1, _rxPin, _txPin);
    delay(1000); // Allow some time for the module to initialize
}

bool LoRa_E5::sendATCommand(const String& command) {
    _serial.print(command + "\r\n");
    delay(100);
    return true;
}

String LoRa_E5::readResponse(uint16_t timeout) {
    String response = "";
    uint32_t start = millis();
    while (millis() - start < timeout) {
        while (_serial.available()) {
            char c = _serial.read();
            response += c;
        }
    }
    return response;
}

bool LoRa_E5::checkResponse(const String& expected) {
    String response = readResponse();
    Serial.println(response); // Print response for debugging
    return response.indexOf(expected) != -1;
}

bool LoRa_E5::initializeModule() {
    sendATCommand("AT");
    return checkResponse("OK");
}

bool LoRa_E5::joinNetwork(const String& devEUI, const String& appEUI, const String& appKey) {
    sendATCommand("AT+ID=DevEui," + devEUI);
    if (!checkResponse("OK")) return false;

    sendATCommand("AT+ID=AppEui," + appEUI);
    if (!checkResponse("OK")) return false;

    sendATCommand("AT+KEY=APPKEY," + appKey);
    if (!checkResponse("OK")) return false;

    sendATCommand("AT+JOIN");
    return checkResponse("JOINED");
}

bool LoRa_E5::sendMessage(const String& data) {
    sendATCommand("AT+CMSGHEX=" + data);
    return checkResponse("OK");
}
