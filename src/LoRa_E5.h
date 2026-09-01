#ifndef LORA_E5_H
#define LORA_E5_H

#include <Arduino.h>

/**
 * Minimal UART/AT-command wrapper for a Seeed LoRa-E5 (Wio-E5) module.
 *
 * The class owns initialisation of the supplied HardwareSerial instance and
 * keeps only the most recent matched response plus signal metrics observed in
 * an uplink response. It does not implement a LoRaWAN stack on the ESP32; the
 * external LoRa-E5 module remains responsible for LoRaWAN protocol behaviour.
 */
class LoRa_E5 {
public:
    /** Configure the UART used to talk to the module. ESP32 RX/TX pins are optional. */
    LoRa_E5(HardwareSerial &serial, uint32_t baud, int8_t rxPin = -1, int8_t txPin = -1);

    /** Open the UART and probe the module with AT until it replies or timeout expires. */
    bool begin(unsigned long timeout = 1000);

    /** Reset the LoRa-E5 through its AT interface. */
    bool initializeModule();

    /** Configure OTAA credentials and wait for the module to report a network join. */
    bool joinNetwork(const String &devEUI, const String &appEUI, const String &appKey, unsigned long timeout = 30000);

    /** Send an even-length hexadecimal uplink on the selected LoRaWAN port. */
    bool sendMessage(const String &data, uint8_t port = 1, bool confirmed = false);

    /** Set the module data-rate index by issuing AT+DR=DR<n>. */
    bool setDataRate(uint8_t dataRate);

    /** Set the module transmit-power value through AT+POWER. */
    bool setTxPower(uint8_t txPower);

    /** Return the firmware/version response from AT+VER, or an empty String on failure. */
    String getDeviceStatus();

    /** Return RSSI captured from the most recent uplink response that contained RSSI. */
    int16_t getRSSI();

    /** Return SNR captured from the most recent uplink response that contained SNR. */
    int8_t getSNR();

private:
    // The serial object is supplied by the sketch; this class does not own its lifetime.
    HardwareSerial &_serial;
    uint32_t _baud;
    int8_t _rxPin;
    int8_t _txPin;

    // The LoRa-E5 reports RSSI/SNR inline in message responses rather than through
    // independent query commands, so these values intentionally represent the last
    // response that contained each metric rather than guaranteed live measurements.
    String _lastResponse;
    int16_t _lastRSSI;
    int8_t _lastSNR;

    // Helpers centralise the line-oriented AT protocol so public methods can focus
    // on the command sequence and expected response tokens for each operation.
    bool sendATCommand(const String &command, const String &expectedResponse, unsigned long timeout = 1000);
    String readResponse(unsigned long timeout = 1000);
    void flushSerial();
    String formatHexPayload(const String &data);
    void captureSignalMetrics(const String &response);
};

#endif // LORA_E5_H
