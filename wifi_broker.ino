#include <WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>
#include <ArduinoJson.h>

const char* ssid = "<ssid>";
const char* password = "<password>";

const char* mqtt_server = "<ipv4server>";

const uint16_t kIrLed = 4;
WiFiClient espClient;

IRSamsungAc ac(kIrLed);
PubSubClient client(espClient);

void printState() {
  // Display the settings.
  Serial.println("Samsung A/C remote is in the following state:");
  Serial.printf("  %s\n", ac.toString().c_str());
  Serial.println();
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  Serial.println();

  JsonDocument doc;
  deserializeJson(doc, (char*) message);

  serializeJson(doc, Serial);
  Serial.println();
  
  if (String(topic) == "esp32/led") {
    doc["on"] ? ac.on() : ac.off();
    ac.setFan(doc["fan"]);
    ac.setMode(doc["mode"]);
    ac.setTemp(doc["temp"]);
    ac.setSwing(doc["swing"]);
    ac.send();
    printState();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("esp32/led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  ac.begin();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}