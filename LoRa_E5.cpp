#include "LoRa_E5.h"

LoRa_E5::LoRa_E5(HardwareSerial &serial, uint32_t baud)
    : _serial(serial), _baud(baud) {}

void LoRa_E5::begin() {
    _serial.begin(_baud);
    delay(1000); // Allow some time for the module to initialize
}

void LoRa_E5::sendATCommand(String command) {
    _serial.print(command + "\r\n"); // Send command with carriage return and newline
    delay(100);
}

String LoRa_E5::readResponse() {
    String response = "";
    while (_serial.available()) {
        char c = _serial.read();
        response += c;
        delay(2); // Small delay to allow all data to be received
    }
    return response;
}

void LoRa_E5::initializeModule() {
    sendATCommand("AT");
    String response = readResponse();
    if (response.indexOf("OK") != -1) {
        Serial.println("Module initialized");
    } else {
        Serial.println("Failed to initialize module");
    }
}

void LoRa_E5::joinNetwork(String devEUI, String appEUI, String appKey) {
    sendATCommand("AT+ID=DevEui," + devEUI);
    sendATCommand("AT+ID=AppEui," + appEUI);
    sendATCommand("AT+KEY=APPKEY," + appKey);
    sendATCommand("AT+JOIN");
    
    String response = readResponse();
    if (response.indexOf("JOINED") != -1) {
        Serial.println("Successfully joined the network");
    } else {
        Serial.println("Failed to join the network");
    }
}

void LoRa_E5::sendMessage(String data) {
    sendATCommand("AT+CMSGHEX=" + data);
    String response = readResponse();
    if (response.indexOf("OK") != -1) {
        Serial.println("Message sent successfully");
    } else {
        Serial.println("Failed to send message");
    }
}
