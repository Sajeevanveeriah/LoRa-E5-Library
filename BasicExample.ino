#include <LoRa_E5.h>

LoRa_E5 lora(LORA_SERIAL, 9600);

void setup() {
    Serial.begin(115200);
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
