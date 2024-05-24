#include "MQTTFuncs.h"


const char ssid[] = "Syntronic Kista";
const char pass[] = "syntronic20Kista";

const char username[] = "christian";
const char passwd[] = "mqttpermission";

MQTTClient client;
WiFiClient net;


unsigned long lastMillis = 0;

void MQTTSetup(){
  WiFi.begin(ssid, pass);
}

void Connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("esp32", username, passwd)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
}

void Pub(const String message){

  client.begin("10.8.96.85", net);
  Connect();
  client.loop();

  if (client.publish("test_sensor_data", message, true, 1)) {
    Serial.println("Message sent successfully");
    client.disconnect();
    Serial.println("Disconnected from MQTT broker");
    WiFi.disconnect();
  } else {
    Serial.println("Failed to send message");
  }
}