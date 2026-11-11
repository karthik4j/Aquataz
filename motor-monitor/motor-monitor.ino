#include <SPI.h>
#include <RF24.h>
#include <SoftwareSerial.h>

// SoftwareSerial Pins: Pin 2 = RX (from ESP TX), Pin 10 = TX (to ESP RX)
SoftwareSerial espSerial(2, 10);

// nRF24L01 Pins (CE=9, CSN=8 or whatever your shield uses)
RF24 radio(7, 8); 

const byte address[6] = "PUMP1";

struct MotorData {
  unsigned long uptime_ms;
  float temperature;
};

MotorData receivedData;

void setup() {
  Serial.begin(9600);    // Debug via USB
  espSerial.begin(9600); // Communication to ESP-01

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening(); // Set as receiver

  Serial.println("[UNO] Gateway Listening for nRF24 packets...");
}

void loop() {
  // 1. Forward incoming ESP debug messages to PC USB Serial
  while (espSerial.available() > 0) {
    Serial.write(espSerial.read());
  }

  // 2. Read incoming wireless nRF24 packets from Nano
  if (radio.available()) {
    radio.read(&receivedData, sizeof(receivedData));

    // Construct JSON payload with PUB: prefix for ESP-01
    String jsonPayload = "PUB:{\"uptime_ms\":" + String(receivedData.uptime_ms) + 
                         ",\"temp\":" + String(receivedData.temperature, 1) + "}";

    // Send payload to ESP-01 over SoftwareSerial
    espSerial.println(jsonPayload);

    // Copy to USB Serial Monitor for confirmation
    Serial.print("[UNO] Forwarded Packet: ");
    Serial.println(jsonPayload);
  }
}