#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

const char* ssid     = "lolpackets-2.4G";
const char* password = "BryceRules";
const char* mqtt_server = "spark4.thedevranch.net";

int sensorThresholdForFlashing = 400;
int delayBetweenFlashes = 100;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH); // turn off the light to start
  Serial.begin(115200);
  /* NETWORK SET UP */
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

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  } 
  client.loop();
  delay(1000); // wait 1s before publishing another value
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      client.subscribe("test");
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& jsonRoot = jsonBuffer.parseObject(payload);
  
  if (!jsonRoot.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  int jsonSensorValue = jsonRoot["sensorValue"];
  Serial.print("jsonSensorValue: ");
  Serial.println(jsonSensorValue);
  
  // Switch on the LED if an 1 was received as first character
  if (jsonSensorValue < sensorThresholdForFlashing) {  
   flashLED();
  }
}

void flashLED() {
  int flashTimes = 25;
  while(flashTimes > 0) {
    Serial.print(" ");
    Serial.print(flashTimes);
    digitalWrite(BUILTIN_LED, LOW);
    delay(delayBetweenFlashes);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(delayBetweenFlashes);
    flashTimes--;
  }
  Serial.println();
}

