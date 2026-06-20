# LoRa-E5 Library for ESP32

## Overview
This library provides a simple interface to communicate with a Seeed LoRa-E5
(Wio-E5) module from an ESP32 over a HardwareSerial UART. It lets you initialise
the module, join a LoRaWAN network using OTAA, and send hex uplinks using the
module's AT command set.

## Features
* Initialise the LoRa-E5 module
* Join a LoRaWAN network (OTAA)
* Send hex messages over LoRaWAN (confirmed or unconfirmed)
* Read the firmware version, and the RSSI/SNR reported by the last uplink

## Hardware Requirements
* ESP32
* Seeed LoRa-E5 / Wio-E5 module
* UART connection between the ESP32 and the LoRa-E5 module (9600 8N1)

## Installation
1. Clone this repository into your Arduino libraries folder:
   ```
   cd ~/Documents/Arduino/libraries
   git clone https://github.com/Sajeevanveeriah/LoRa-E5-Library.git
   ```
2. Restart the Arduino IDE if it was open.

## Usage Example
The constructor takes the serial port, baud rate and (for ESP32) the RX and TX
pins, so the library opens the port for you in `begin()`. Do not call
`Serial1.begin()` yourself as well.

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
        Serial.println("Failed to initialise LoRa-E5 module");
        while (1);
    }

    String devEUI = "0000000000000000"; // Replace with your DevEUI
    String appEUI = "0000000000000000"; // Replace with your AppEUI
    String appKey = "00000000000000000000000000000000"; // Replace with your AppKey

    if (!lora.joinNetwork(devEUI, appEUI, appKey)) {
        Serial.println("Failed to join network");
        while (1);
    }

    Serial.println("Successfully joined network");
}

void loop() {
    String data = "48656C6C6F576F726C64"; // "HelloWorld" in hex
    lora.sendMessage(data);
    delay(60000); // Wait for 1 minute before sending the next message
}
```

A complete sketch is in `examples/BasicExample/BasicExample.ino`.

### Payload format
`sendMessage()` takes the payload as a hex string. It must contain an even
number of hex digits. Contiguous hex (for example `48656C6C6F`) and
space-separated hex (`48 65 6C 6C 6F`) are both accepted; the library reformats
the payload into the space-separated form the module expects before sending. Any
other input is rejected and `sendMessage()` returns `false`.

## API

### `LoRa_E5(HardwareSerial &serial, uint32_t baud, int8_t rxPin = -1, int8_t txPin = -1)`
Constructor.
- `serial`: the HardwareSerial instance (for example `Serial1`).
- `baud`: the baud rate (the LoRa-E5 default is 9600).
- `rxPin`, `txPin`: the ESP32 UART pins. If left at -1 the default pins for that
  port are used.

### `bool begin(unsigned long timeout = 1000)`
Opens the serial port and waits for the module to answer `AT` with `OK`. Returns
`true` once the module responds.

### `bool initializeModule()`
Sends `AT+RESET` to the module.

### `bool joinNetwork(const String &devEUI, const String &appEUI, const String &appKey, unsigned long timeout = 30000)`
Sets OTAA mode (`AT+MODE=LWOTAA`), sets the quoted credentials, sends `AT+JOIN`,
then waits up to `timeout` for `+JOIN: Network joined`. Returns `true` on a
successful join, `false` on `+JOIN: Join failed` or timeout.

### `bool sendMessage(const String &data, uint8_t port = 1, bool confirmed = false)`
Sends `data` (a hex string, see Payload format) as an uplink. When `confirmed`
is `true` it uses `AT+CMSGHEX`, otherwise `AT+MSGHEX`. When `port` is not 1 it
sets the port with `AT+PORT` first. Returns `true` when the module reports
`Done`.

### `bool setDataRate(uint8_t dataRate)`
Sets the data rate, for example `setDataRate(3)` sends `AT+DR=DR3`.

### `bool setTxPower(uint8_t txPower)`
Sets the transmit power with `AT+POWER`.

### `String getDeviceStatus()`
Returns the firmware version string reported by `AT+VER`.

### `int16_t getRSSI()`
Returns the RSSI parsed from the most recent uplink response. It is only
meaningful after a `sendMessage()` whose response carried an RSSI value
(typically a confirmed uplink, or any uplink with a downlink).

### `int8_t getSNR()`
Returns the SNR parsed from the most recent uplink response, with the same
caveat as `getRSSI()`.

## Hardware Smoke Test
This library is verified to compile offline for ESP32. The steps below need real
hardware and a LoRaWAN network server, so run them yourself:

1. Wire the ESP32 UART2 to the LoRa-E5 RX/TX, with a common ground, at 9600 8N1.
2. Confirm `AT` returns `OK`, and `AT+VER` returns a version (note it down, as
   some behaviour is firmware dependent).
3. Run `joinNetwork()` with your real OTAA keys and confirm it reaches
   `+JOIN: Network joined`.
4. Run `sendMessage()` with a known hex string, confirm it returns `Done`, and
   confirm the uplink appears on your network server.

## Contributing
Contributions are welcome. Please fork this repository and submit pull requests.

## License
This project is licensed under the Unlicense.

## Contact
If you have any questions or feedback, feel free to reach out.

GitHub: https://github.com/Sajeevanveeriah
