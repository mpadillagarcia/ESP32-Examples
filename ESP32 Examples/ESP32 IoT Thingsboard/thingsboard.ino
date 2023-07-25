#include "ThingsBoard.h"
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_ADXL343.h>
#include <Adafruit_Sensor.h>
#include <type_traits>


#define WIFI_AP "wifi ssid"
#define WIFI_PASSWORD "wifi pass"

#define TOKEN "thingsboard token"
#define THINGSBOARD_SERVER "thingsboard server"

#define SERIAL_DEBUG_BAUD 115200
WiFiClient espClient;
ThingsBoard tb(espClient);

int status = WL_IDLE_STATUS;

Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);


const int ledPin = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(SERIAL_DEBUG_BAUD);
  while (!Serial);
  Serial.println("Accelerometer Test"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_16_G);

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  delay(1000);
  InitWiFi();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}


void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  if(WiFi.status() != WL_CONNECTED){
    reconnect();
  }

  if (!tb.connected()){
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);

    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
    digitalWrite(ledPin, HIGH);
  }

  sensors_event_t event;
    accel.getEvent(&event);

  Serial.println("Sending data...");
  tb.sendTelemetryFloat("X", event.acceleration.x);
  tb.sendTelemetryFloat("Y", event.acceleration.y);
  tb.sendTelemetryFloat("Z", event.acceleration.z);

  delay(1000);
  tb.loop();
}

void InitWiFi(){
  Serial.println("Connecting to AP...");
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  if(status != WL_CONNECTED){
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}
