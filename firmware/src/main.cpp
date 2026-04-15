#include <Arduino.h>

static constexpr uint32_t USB_BAUD = 115200;
static constexpr uint32_t PING_INTERVAL_MS = 2000;

unsigned long lastPing = 0;
String rxBuffer;

void setup() {
    Serial.begin(USB_BAUD);
}

void loop() {
    unsigned long now = millis();
    if (now - lastPing >= PING_INTERVAL_MS) {
        lastPing = now;
        Serial.println("PING");
    }

    while (Serial.available() > 0) {
        char ch = static_cast<char>(Serial.read());

        if (ch == '\r') {
            continue;
        }

        if (ch == '\n') {
            if (!rxBuffer.isEmpty()) {
                Serial.print("RX: ");
                Serial.println(rxBuffer);
                rxBuffer.clear();
            }
            continue;
        }

        rxBuffer += ch;
    }
}
