// ESP32_4_Channel_Lighting.ino
// Complete code for ESP32 + 4-Channel Relay Module controlling 4 LED Bulbs

#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT Broker (Free Cloud Broker)
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

// Relay control pins (GPIO)
const int relayPins[4] = {13, 12, 14, 27};

// Light configuration
struct Light {
  int id;
  String name;
  String room;
  bool state;
  int pin;
};

Light lights[4] = {
  {1, "Light 1", "Living Room", false, 13},
  {2, "Light 2", "Bedroom", false, 12},
  {3, "Light 3", "Kitchen", false, 14},
  {4, "Light 4", "Study Room", false, 27}
};

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastHeartbeat = 0;
bool mqttConnected = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32 Smart Lighting System Starting...");

  // Initialize relay pins
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
    Serial.print("Initialized ");
    Serial.println(lights[i].name);
  }

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWi-Fi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Message received: ");
  Serial.print(topic);
  Serial.print(" -> ");
  Serial.println(message);
  
  String topicStr = String(topic);
  
  // Individual light control
  if (topicStr.startsWith("home/light/")) {
    int firstSlash = topicStr.indexOf("/", 10);
    int lightId = topicStr.substring(10, firstSlash).toInt();
    
    if (lightId >= 1 && lightId <= 4) {
      bool turnOn = (message == "ON");
      controlLight(lightId, turnOn);
      publishLightStatus(lightId);
    }
  }
  // All lights control
  else if (topicStr == "home/all/command") {
    if (message == "ON") {
      for (int i = 0; i < 4; i++) {
        controlLight(i + 1, true);
      }
      publishAllStatus();
    }
    else if (message == "OFF") {
      for (int i = 0; i < 4; i++) {
        controlLight(i + 1, false);
      }
      publishAllStatus();
    }
  }
}

void controlLight(int lightId, bool turnOn) {
  int index = lightId - 1;
  if (turnOn) {
    digitalWrite(relayPins[index], HIGH);
    lights[index].state = true;
    Serial.print(lights[index].name);
    Serial.println(" turned ON");
  } else {
    digitalWrite(relayPins[index], LOW);
    lights[index].state = false;
    Serial.print(lights[index].name);
    Serial.println(" turned OFF");
  }
}

void publishLightStatus(int lightId) {
  int index = lightId - 1;
  String topic = "home/light/" + String(lightId) + "/status";
  String payload = lights[index].state ? "ON" : "OFF";
  client.publish(topic.c_str(), payload.c_str());
}

void publishAllStatus() {
  for (int i = 0; i < 4; i++) {
    String topic = "home/light/" + String(i + 1) + "/status";
    String payload = lights[i].state ? "ON" : "OFF";
    client.publish(topic.c_str(), payload.c_str());
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    String clientId = "ESP32_Lighting_";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected");
      mqttConnected = true;
      
      // Subscribe to individual light commands
      for (int i = 1; i <= 4; i++) {
        String topic = "home/light/" + String(i) + "/command";
        client.subscribe(topic.c_str());
      }
      // Subscribe to all lights command
      client.subscribe("home/all/command");
      
      publishAllStatus();
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}