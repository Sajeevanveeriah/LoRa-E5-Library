# LoRa-E5 Library for ESP32

## Overview
This library provides a simple interface to communicate with the LoRa-E5 module using an ESP32. It allows you to initialize the module, join a LoRaWAN network, and send messages using AT commands.

## Features
* Initialize the LoRa-E5 module
* Join a LoRaWAN network
* Send messages over LoRaWAN

## Hardware Requirements
* ESP32
* LoRa-E5 Module
* UART connection between ESP32 and LoRa-E5 Module

## Installation
1. Clone this repository into your Arduino libraries folder:
   ```
   cd ~/Documents/Arduino/libraries
   git clone https://github.com/yourusername/LoRa_E5_Library.git
   ```
2. Restart the Arduino IDE if it was open.

## Usage Example
Here is an example sketch demonstrating how to use the library:

```cpp
#include <LoRa_E5.h>

// Define the serial interface for LoRa-E5
#define LORA_SERIAL Serial1
#define BAUD_RATE 9600
#define RX_PIN 16
#define TX_PIN 17

LoRa_E5 lora(LORA_SERIAL, BAUD_RATE);

void setup() {
    Serial.begin(115200);
    LORA_SERIAL.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
    delay(1000); // Allow some time for the module to initialize

    lora.begin();
    lora.initializeModule();

    String devEUI = "0000000000000000"; // Replace with your DevEUI
    String appEUI = "0000000000000000"; // Replace with your AppEUI
    String appKey = "00000000000000000000000000000000"; // Replace with your AppKey
    lora.joinNetwork(devEUI, appEUI, appKey);
}

void loop() {
    String data = "HelloWorld"; // Your data in HEX format
    lora.sendMessage(data);
    delay(10000); // Wait for 10 seconds before sending the next message
}
```

## API

### `LoRa_E5(HardwareSerial &serial, uint32_t baud)`
Constructor. Initializes the LoRa_E5 object.
- `serial`: The HardwareSerial instance (e.g., Serial1).
- `baud`: The baud rate for serial communication.

### `void begin()`
Starts the serial communication.

### `void sendATCommand(String command)`
Sends an AT command to the LoRa-E5 module.
- `command`: The AT command string.

### `String readResponse()`
Reads the response from the LoRa-E5 module.

### `void initializeModule()`
Initializes the LoRa-E5 module by sending the AT command.

### `void joinNetwork(String devEUI, String appEUI, String appKey)`
Joins a LoRaWAN network using the provided device EUI, application EUI, and application key.
- `devEUI`: The device EUI.
- `appEUI`: The application EUI.
- `appKey`: The application key.

### `void sendMessage(String data)`
Sends a message over the LoRaWAN network.
- `data`: The data to send, in HEX format.

## Contributing
Contributions are welcome! Please fork this repository and submit pull requests.

## License
This project is licensed under the Unlicensed License.

## Contact
If you have any questions or feedback, feel free to reach out.

GitHub: https://github.com/Sajeevanveeriah
