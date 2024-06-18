#include <LoRa_E5.h>

// Setup the serial communication parameters
HardwareSerial mySerial(1); // Use UART1
int rxPin = 16; // Example RX Pin
int txPin = 17; // Example TX Pin

// Create an instance of the LoRa_E5 class
LoRa_E5 loraModule(mySerial, 9600, rxPin, txPin);

void setup() {
  // Begin Serial Communication
  Serial.begin(115200);
  while (!Serial) continue; // Wait for Serial to be ready

  // Begin communication with the LoRa module
  loraModule.begin();

  // Initialize the LoRa-E5 module
  String response;
  if (!loraModule.sendATCommand("AT", response)) {
    Serial.println("Failed to initialize the LoRa-E5 module.");
    Serial.println("Response: " + response);
  } else {
    Serial.println("LoRa-E5 module initialized successfully.");
  }

  // Join the network using OTAA
  String devEUI = "your_dev_eui";
  String appEUI = "your_app_eui";
  String appKey = "your_app_key";
  if (!loraModule.joinNetwork(devEUI, appEUI, appKey)) {
    Serial.println("Failed to join the network.");
  } else {
    Serial.println("Joined the network successfully.");
  }
}

void loop() {
  // Send a LoRa message periodically
  String data = "Hello, LoRa!";
  if (!loraImodule.sendMessage(data)) {
    Serial.println("Failed to send the message.");
  } else {
    Serial.println("Message sent successfully.");
  }

  // Wait for 10 seconds before sending the next message
  delay(10000);
}
