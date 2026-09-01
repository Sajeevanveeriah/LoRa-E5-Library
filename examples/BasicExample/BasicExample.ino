#include <LoRa_E5.h>

// This sketch demonstrates the library's ownership boundary: the sketch selects
// the ESP32 HardwareSerial port and pins, while LoRa_E5::begin() configures that
// port and verifies the external LoRa-E5 module responds to AT commands.
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

    Serial.print("Firmware version: ");
    Serial.println(lora.getDeviceStatus());

    // These placeholders must be replaced with OTAA credentials issued by the
    // LoRaWAN network. Keep real AppKeys out of source control.
    String devEUI = "0000000000000000";
    String appEUI = "0000000000000000";
    String appKey = "00000000000000000000000000000000";

    // joinNetwork selects LWOTAA mode, configures credentials and then waits for
    // the module's asynchronous +JOIN result rather than a simple OK response.
    if (!lora.joinNetwork(devEUI, appEUI, appKey)) {
        Serial.println("Failed to join network");
        while (1);
    }

    Serial.println("Successfully joined network");
}

void loop() {
    // sendMessage accepts hexadecimal payload bytes. "HelloWorld" is encoded
    // here only to make the example easy to recognise at the network server.
    String data = "48656C6C6F576F726C64";
    if (lora.sendMessage(data)) {
        Serial.println("Message sent successfully");

        // RSSI and SNR are cached from the most recent uplink response that
        // contained those fields; they are not independent live radio queries.
        Serial.print("RSSI: ");
        Serial.println(lora.getRSSI());

        Serial.print("SNR: ");
        Serial.println(lora.getSNR());
    } else {
        Serial.println("Failed to send message");
    }

    // The interval is an application choice, not a library requirement. Real
    // deployments must respect the applicable LoRaWAN regional/duty-cycle rules.
    delay(60000);
}
