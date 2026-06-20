#include "LoRa_E5.h"

LoRa_E5::LoRa_E5(HardwareSerial &serial, uint32_t baud, int8_t rxPin, int8_t txPin)
    : _serial(serial), _baud(baud), _rxPin(rxPin), _txPin(txPin), _lastRSSI(0), _lastSNR(0) {}

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
    // OTAA mode must be selected before the credentials are set.
    if (!sendATCommand("AT+MODE=LWOTAA", "+MODE: LWOTAA")) return false;

    // Credentials are quoted; the module echoes them back rather than "OK".
    if (!sendATCommand("AT+ID=DevEui,\"" + devEUI + "\"", "+ID: DevEui")) return false;
    if (!sendATCommand("AT+ID=AppEui,\"" + appEUI + "\"", "+ID: AppEui")) return false;
    if (!sendATCommand("AT+KEY=APPKEY,\"" + appKey + "\"", "+KEY: APPKEY")) return false;

    // AT+JOIN does not return "OK"; it streams progress lines instead.
    flushSerial();
    _serial.println("AT+JOIN");

    unsigned long startTime = millis();
    while (millis() - startTime < timeout) {
        String response = readResponse(1000);
        if (response.indexOf("+JOIN: Network joined") != -1) {
            return true;
        }
        if (response.indexOf("+JOIN: Join failed") != -1) {
            return false;
        }
    }
    return false;
}

bool LoRa_E5::sendMessage(const String &data, uint8_t port, bool confirmed) {
    // Payload must be even-length hex, sent as space-separated bytes.
    String payload = formatHexPayload(data);
    if (payload.length() == 0) return false;

    // There is no port argument on the message command; set it separately.
    if (port != 1) {
        sendATCommand("AT+PORT=" + String(port), "+PORT:");
    }

    // Payload is quoted. Confirmed and unconfirmed use different tokens.
    String cmd = confirmed ? "AT+CMSGHEX=\"" : "AT+MSGHEX=\"";
    cmd += payload + "\"";
    String token = confirmed ? "+CMSGHEX" : "+MSGHEX";

    flushSerial();
    _serial.println(cmd);

    // An uplink spans both receive windows, so allow more time than a plain AT.
    unsigned long startTime = millis();
    while (millis() - startTime < 15000) {
        String response = readResponse(1000);
        captureSignalMetrics(response);
        if (response.indexOf(token) != -1 && response.indexOf("Done") != -1) {
            return true;
        }
        if (response.indexOf("fail") != -1 || response.indexOf("ERROR") != -1) {
            return false;
        }
    }
    return false;
}

bool LoRa_E5::setDataRate(uint8_t dataRate) {
    // The data rate is set as DR<n>, e.g. AT+DR=DR3, and echoed back as "+DR:".
    return sendATCommand("AT+DR=DR" + String(dataRate), "+DR:");
}

bool LoRa_E5::setTxPower(uint8_t txPower) {
    return sendATCommand("AT+POWER=" + String(txPower), "OK");
}

String LoRa_E5::getDeviceStatus() {
    // There is no AT+STATUS on this module; AT+VER is the real status query.
    if (sendATCommand("AT+VER", "+VER:")) {
        String version = _lastResponse;
        version.trim();
        return version;
    }
    return "";
}

int16_t LoRa_E5::getRSSI() {
    // There is no AT+RSSI; this is the value from the most recent uplink.
    return _lastRSSI;
}

int8_t LoRa_E5::getSNR() {
    // There is no AT+SNR; this is the value from the most recent uplink.
    return _lastSNR;
}

bool LoRa_E5::sendATCommand(const String &command, const String &expectedResponse, unsigned long timeout) {
    flushSerial();
    _serial.println(command);
    
    unsigned long startTime = millis();
    while (millis() - startTime < timeout) {
        if (_serial.available()) {
            String response = readResponse(timeout);
            if (response.indexOf(expectedResponse) != -1) {
                _lastResponse = response;
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

String LoRa_E5::formatHexPayload(const String &data) {
    // Drop any spaces, validate the hex, then regroup into space-separated bytes,
    // e.g. "48656C6C6F" becomes "48 65 6C 6C 6F". Returns "" if the input is not
    // valid even-length hex.
    String hex = "";
    for (unsigned int i = 0; i < data.length(); i++) {
        char c = data.charAt(i);
        if (c == ' ') continue;
        if (!isHexadecimalDigit(c)) return "";
        hex += c;
    }
    if (hex.length() == 0 || (hex.length() % 2) != 0) return "";

    String out = "";
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        if (i > 0) out += ' ';
        out += hex.charAt(i);
        out += hex.charAt(i + 1);
    }
    return out;
}

void LoRa_E5::captureSignalMetrics(const String &response) {
    // RSSI and SNR are reported inline inside uplink responses, e.g.
    //   +MSGHEX: RXWIN1, RSSI -106, SNR 4
    int r = response.indexOf("RSSI");
    if (r != -1) {
        _lastRSSI = (int16_t) response.substring(r + 4).toInt();
    }
    int s = response.indexOf("SNR");
    if (s != -1) {
        _lastSNR = (int8_t) response.substring(s + 3).toFloat();
    }
}
