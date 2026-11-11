#include <SPI.h>
#include <RF24.h>

// nRF24L01 Pins (CE, CSN)
RF24 radio(8, 7); 

// 6-byte pipe address (must match receiver)
const byte address[6] = "PUMP1";

// Data Structure (Max 32 bytes for nRF24L01)
struct MotorData {
  unsigned long uptime_ms;
  float temperature; // Dummy temperature for future thermistor
};

MotorData payload;
unsigned long lastSend = 0;

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW); // Set to RF24_PA_MAX if nodes are far apart
  radio.stopListening();         // Set as transmitter
}

void loop() {
  unsigned long now = millis();

  // Send packet every 2 seconds
  if (now - lastSend >= 2000) {
    lastSend = now;

    payload.uptime_ms = now;
    payload.temperature = 25.0; // Dummy value (e.g. 25.0 C)

    // Transmit packet wirelessly
    radio.write(&payload, sizeof(payload));
  }
}