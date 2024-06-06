#include "LoRa_E5.h"

LoRa_E5::LoRa_E5(HardwareSerial &serial, uint32_t baud, int8_t rxPin, int8_t txPin)
    : _serial(serial), _baud(baud), _rxPin(rxPin), _txPin(txPin) {}

void LoRa_E5::begin() {
    _serial.begin(_baud, SERIAL_8N1, _rxPin, _txPin);
    delay(1000); // Allow some time for the module to initialize
}

bool LoRa_E5::sendATCommand(const String& command, String& response) {
    _serial.print(command + "\\r\\n");
    response = readResponse(2000); // Increased timeout for longer responses
    if (response.indexOf("OK") != -1) {
        return true; // Command succeeded
    } else if (response.indexOf("ERROR") != -1) {
        Serial.println("Error response: " + response);
        return false; // Command failed with an error
    }
    Serial.println("Unexpected response: " + response);
    return false; // Handle other unexpected responses
}

String LoRa_E5::readResponse(uint16_t timeout) {
    String response = "";
    uint32_t start = millis();
    while (millis() - start < timeout) {
        while (_serial.available()) {
            char c = _serial.read();
            response += c;
            if (response.endsWith("\\r\\n")) {
                return response; // Return after each complete line
            }
        }
    }
    return response; // Return whatever was received after timeout
}

bool Loa_TRY{
...
    return false;
}
