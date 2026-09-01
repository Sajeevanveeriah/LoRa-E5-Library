# LoRa-E5 Library for ESP32

Arduino/ESP32 wrapper for communicating with a Seeed LoRa-E5 (Wio-E5) module over `HardwareSerial`. The ESP32 sends the module's AT commands; the LoRa-E5 itself owns the LoRaWAN protocol stack.

## Scope

The library can:

- initialise and probe the module;
- reset it through `AT+RESET`;
- configure OTAA credentials and join a LoRaWAN network;
- send confirmed or unconfirmed hexadecimal uplinks;
- set data rate and transmit power;
- read the firmware/version response;
- retain RSSI and SNR values reported inline by the most recent relevant uplink response.

It does not provision a LoRaWAN network server, securely store OTAA keys or implement the LoRaWAN MAC on the ESP32.

## Hardware

- ESP32
- Seeed LoRa-E5 / Wio-E5
- UART connection between the devices
- Common ground
- Default module UART setting: 9600 baud, 8N1

## Installation

Clone or copy this repository into the Arduino libraries directory, then restart the Arduino IDE if required.

```bash
cd ~/Documents/Arduino/libraries
git clone https://github.com/Sajeevanveeriah/LoRa-E5-Library.git
```

## Basic use

```cpp
#include <LoRa_E5.h>

#define LORA_SERIAL Serial1
#define BAUD_RATE 9600
#define RX_PIN 16
#define TX_PIN 17

LoRa_E5 lora(LORA_SERIAL, BAUD_RATE, RX_PIN, TX_PIN);

void setup() {
    Serial.begin(115200);

    if (!lora.begin()) {
        while (1);
    }

    String devEUI = "0000000000000000";
    String appEUI = "0000000000000000";
    String appKey = "00000000000000000000000000000000";

    if (!lora.joinNetwork(devEUI, appEUI, appKey)) {
        while (1);
    }
}

void loop() {
    lora.sendMessage("48656C6C6F");
    delay(60000);
}
```

Use network-issued OTAA credentials in a real deployment and keep secret keys out of source control. A fuller sketch is provided in `examples/BasicExample/BasicExample.ino`.

## Payload format

`sendMessage()` accepts hexadecimal bytes. Contiguous hex such as `48656C6C6F` and space-separated hex such as `48 65 6C 6C 6F` are accepted. Input must resolve to an even number of hexadecimal digits. Invalid input returns `false` before an uplink command is issued.

## API behaviour

### `LoRa_E5(HardwareSerial &serial, uint32_t baud, int8_t rxPin = -1, int8_t txPin = -1)`

Stores the serial interface and UART settings. `begin()` configures the supplied `HardwareSerial`, so do not independently initialise the same port with conflicting settings.

### `bool begin(unsigned long timeout = 1000)`

Opens the UART and repeatedly probes the module with `AT` until `OK` is observed or the timeout expires.

### `bool initializeModule()`

Sends `AT+RESET` and expects `OK`.

### `bool joinNetwork(...)`

Selects OTAA mode, writes DevEUI/AppEUI/AppKey and sends `AT+JOIN`. Join completion is asynchronous: the implementation waits for `+JOIN: Network joined`, `+JOIN: Join failed` or timeout.

### `bool sendMessage(const String &data, uint8_t port = 1, bool confirmed = false)`

Validates and formats the payload, optionally changes the port, then uses `AT+MSGHEX` or `AT+CMSGHEX`. The method waits for the module's completion/failure output and captures RSSI/SNR when present.

### `setDataRate()` and `setTxPower()`

Pass the requested values to the module's AT interface. The library does not decide whether a value is appropriate for the deployment's LoRaWAN region or network policy.

### `getDeviceStatus()`

Returns the `AT+VER` response, despite the historical method name `getDeviceStatus()`.

### `getRSSI()` and `getSNR()`

Return cached metrics parsed from the most recent uplink response containing them. They are not live measurement queries and may remain at a previous value when a later response omits those fields.

## Repository map

See [`CODE-MAP.md`](CODE-MAP.md) before changing the library. In particular, the public header, AT parser, Arduino metadata and example sketch have different compatibility responsibilities.

## Verification

For changes to commands or parsing, use both software and hardware checks where available:

1. compile the library and `BasicExample` for the intended ESP32 core;
2. confirm `AT` returns `OK` on real hardware;
3. record `AT+VER` output because behaviour can vary with module firmware;
4. join with valid OTAA credentials;
5. transmit a known payload and confirm it arrives at the network server;
6. verify confirmed/unconfirmed paths and non-default ports if they were touched;
7. validate RSSI/SNR parsing against the actual response format.

A documentation-only change does not constitute a hardware validation.

## Maintenance rules

Comments should explain AT response semantics, timing, UART ownership, payload validation and radio/network assumptions. Avoid comments that merely repeat the C++ syntax.

## Licence

See `LICENSE` for the repository licence.
