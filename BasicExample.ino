#include <LoRa_E5.h>

// Define the serial interface for LoRa-E5
#define LORA_SERIAL Serial1
#define BAUD_RATE 9600
#define RX_PIN 16 // Replace with your RX pin
#define TX_PIN 17 // Replace with your TX pin

LoRa_E5 lora(LORA_SERIAL, BAUD_RATE, RX_PIN, TX_PIN);

void setup() {
    Serial.begin(115200);
    lora.begin();

    if (!lora.initializeModule()) {
        Serial.println("Failed to initialize module");
        return;
    }

    String devEUI = "0000000000000000"; // Replace with your DevEUI
    String appEUI = "0000000000000000"; // Replace with your AppEUI
    String appKey = "00000000000000000000000000000000"; // Replace with your AppKey

    if (!lora.joinNetwork(devEUI, appEUI, appKey)) {
        Serial.println("Failed to join the network");
        return;
    }
}

void loop() {
    String data = "HelloWorld"; // Your data in HEX format
    if (!lora.sendMessage(data)) {
        Serial.println("Failed to send message");
    }
    delay(10000); // Wait for 10 seconds before sending the next message
}
