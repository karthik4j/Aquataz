#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// --- Configuration Settings ---
const char* WIFI_SSID     = "x;       // Your 2.4GHz Wi-Fi Name
const char* WIFI_PASS     = "y";   // Your Wi-Fi Password
const char* MQTT_SERVER   = "z";          // Your Windows PC's Local IP address
const int   MQTT_PORT     = 1883;
const char* MQTT_TOPIC    = "waterpump/test";

const int LED_PIN = 2; // Onboard LED

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // OFF

  Serial.begin(9600); // Matches Uno SoftwareSerial rate

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, LOW); delay(100); digitalWrite(LED_PIN, HIGH); delay(400);
  }

  client.setServer(MQTT_SERVER, MQTT_PORT);
}

void reconnectMQTT() {
  while (!client.connected()) {
    if (client.connect("ESP01_Gateway")) {
      digitalWrite(LED_PIN, LOW); // Solid ON when connected
    } else {
      digitalWrite(LED_PIN, HIGH);
      delay(3000);
    }
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (!client.connected()) reconnectMQTT();

  client.loop();

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.startsWith("PUB:")) {
      String payload = input.substring(4);
      client.publish(MQTT_TOPIC, payload.c_str());
    }
  }
}