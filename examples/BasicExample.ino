#include <LoRa_E5.h>

#define LORA_SERIAL Serial1
#define BAUD_RATE 9600
#define RX_PIN 16
#define TX_PIN 17

LoRa_E5 lora(LORA_SERIAL, BAUD_RATE, RX_PIN, TX_PIN);

void setup() {
    Serial.begin(115200);
    
    if (!lora.begin()) {
        Serial.println("Failed to initialize LoRa-E5 module");
        while (1);
    }

    if (!lora.initializeModule()) {
        Serial.println("Failed to reset LoRa-E5 module");
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
    String data = "48656C6C6F576F726C64"; // "HelloWorld" in HEX format
    if (lora.sendMessage(data)) {
        Serial.println("Message sent successfully");
    } else {
        Serial.println("Failed to send message");
    }

    // Example of using new methods
    Serial.print("Battery voltage: ");
    Serial.println(lora.getBatteryVoltage());
    
    Serial.print("RSSI: ");
    Serial.println(lora.getRSSI());

    Serial.print("SNR: ");
    Serial.println(lora.getSNR());

    delay(60000); // Wait for 1 minute before sending the next message
}
