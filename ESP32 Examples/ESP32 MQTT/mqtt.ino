#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_ADXL343.h>
#include <Adafruit_Sensor.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "wifi ssid";
const char* password = "wifi pass";
WiFiServer server(80);
// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "mqtt server ip";
const char* mqtt_server = "192.168.1.*";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);
float X = 0;
float Y = 0;
float Z = 0;
// LED Pin
const int ledPin = 4;
const int ledPinAzul = 2;

void setup() {
  Serial.begin(115200);
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  //status = bme.begin();  
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }
  accel.setRange(ADXL343_RANGE_16_G);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
 

  pinMode(ledPin, OUTPUT);
  pinMode(ledPinAzul, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
  
    sensors_event_t event;
    accel.getEvent(&event);

    X = event.acceleration.x; 
    // Convert the value to a char array
    char axisxString[8];
    dtostrf(X, 1, 2, axisxString);
    Serial.print("X: ");
    Serial.println(axisxString);
    

    Y = event.acceleration.y; 
    // Convert the value to a char array
    char axisyString[8];
    dtostrf(Y, 1, 2, axisyString);
    Serial.print("Y: ");
    Serial.println(axisyString);
    

        Z = event.acceleration.z; 
    // Convert the value to a char array
    char axiszString[8];
    dtostrf(Z, 1, 2, axiszString);
    Serial.print("Z: ");
    Serial.println(axiszString);
    

    digitalWrite(ledPinAzul, HIGH);
    client.publish("esp32/X", axisxString);
    client.publish("esp32/Y", axisyString);
    client.publish("esp32/Z", axiszString);
    delay(1000);
    digitalWrite(ledPinAzul, LOW);
  }
}
 
