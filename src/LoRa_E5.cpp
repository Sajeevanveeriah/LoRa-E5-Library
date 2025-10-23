#include "LoRa_E5.h"

LoRa_E5::LoRa_E5(HardwareSerial &serial, uint32_t baud, int8_t rxPin, int8_t txPin)
    : _serial(serial), _baud(baud), _rxPin(rxPin), _txPin(txPin) {}

bool LoRa_E5::begin(unsigned long timeout) {
    if (_rxPin != -1 && _txPin != -1) {
        _serial.begin(_baud, SERIAL_8N1, _rxPin, _txPin);
    } else {
        _serial.begin(_baud);
    }

    unsigned long startTime = millis();
    while (millis() - startTime < timeout) {
        if (sendATCommand("AT", "OK")) {
            return true;
        }
        delay(100);
    }
    return false;
}

bool LoRa_E5::initializeModule() {
    return sendATCommand("AT+RESET", "OK");
}

bool LoRa_E5::joinNetwork(const String &devEUI, const String &appEUI, const String &appKey, unsigned long timeout) {
    if (!sendATCommand("AT+ID=DevEui," + devEUI, "OK")) return false;
    if (!sendATCommand("AT+ID=AppEui," + appEUI, "OK")) return false;
    if (!sendATCommand("AT+KEY=APPKEY," + appKey, "OK")) return false;
    
    if (!sendATCommand("AT+JOIN", "OK")) return false;

    unsigned long startTime = millis();
    while (millis() - startTime < timeout) {
        String response = readResponse(1000);
        if (response.indexOf("+JOIN: Network joined") != -1) {
            return true;
        }
    }
    return false;
}

bool LoRa_E5::sendMessage(const String &data, uint8_t port, bool confirmed) {
    String cmd = confirmed ? "AT+CMSGHEX=" : "AT+MSGHEX=";
    cmd += data + "," + String(port);
    
    return sendATCommand(cmd, "+MSGHEX: Done");
}

bool LoRa_E5::setDataRate(uint8_t dataRate) {
    return sendATCommand("AT+DR=" + String(dataRate), "OK");
}

bool LoRa_E5::setTxPower(uint8_t txPower) {
    return sendATCommand("AT+POWER=" + String(txPower), "OK");
}

String LoRa_E5::getDeviceStatus() {
    if (sendATCommand("AT+STATUS", "+STATUS:")) {
        return readResponse();
    }
    return "";
}

float LoRa_E5::getBatteryVoltage() {
    if (sendATCommand("AT+BAT", "+BAT:")) {
        String response = readResponse();
        return response.toFloat();
    }
    return -1.0;
}

int16_t LoRa_E5::getRSSI() {
    if (sendATCommand("AT+RSSI", "+RSSI:")) {
        String response = readResponse();
        return response.toInt();
    }
    return 0;
}

int8_t LoRa_E5::getSNR() {
    if (sendATCommand("AT+SNR", "+SNR:")) {
        String response = readResponse();
        return response.toInt();
    }
    return 0;
}

bool LoRa_E5::sendATCommand(const String &command, const String &expectedResponse, unsigned long timeout) {
    flushSerial();
    _serial.println(command);
    
    unsigned long startTime = millis();
    while (millis() - startTime < timeout) {
        if (_serial.available()) {
            String response = readResponse(timeout);
            if (response.indexOf(expectedResponse) != -1) {
                return true;
            }
        }
    }
    return false;
}

String LoRa_E5::readResponse(unsigned long timeout) {
    String response = "";
    unsigned long startTime = millis();
    while (millis() - startTime < timeout) {
        if (_serial.available()) {
            char c = _serial.read();
            response += c;
            if (c == '\n') {
                return response;
            }
        }
    }
    return response;
}

void LoRa_E5::flushSerial() {
    while (_serial.available()) {
        _serial.read();
    }
}
