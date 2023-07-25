#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>


BLEServer* pServer = NULL;
BLECharacteristic* EjeX = NULL;
BLECharacteristic* EjeY = NULL;
BLECharacteristic* EjeZ = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

char xValue[6];
char yValue[6];
char zValue[6];

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_X "xeb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_Y "yeb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_Z "zeb5483e-36e1-4688-b7f5-ea07361b26a8"


int pin_dos = 2;

Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};



void setup() {
  Serial.begin(115200);
  pinMode(pin_dos, OUTPUT);
  // Create the BLE Device
  BLEDevice::init("Grupo 2 ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  EjeX = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_X,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  EjeY = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_Y,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  EjeZ = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_Z,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // Start the service
  pService->start();

  if(!accel.begin())
  {
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }

  accel.setRange(ADXL343_RANGE_16_G);
  accel.printSensorDetails();
  Serial.println("");

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
    // notify changed value
    if (deviceConnected) {
       
        
        
        delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms

        sensors_event_t event;
        accel.getEvent(&event);
       
        dtostrf(event.acceleration.x, 3, 2, xValue);
        EjeX->setValue(xValue);
        EjeX->notify();
        delay(3);

        dtostrf(event.acceleration.y, 3, 2, yValue);
        EjeY->setValue(yValue);
        EjeY->notify();
        delay(3);
        
        dtostrf(event.acceleration.z, 3, 2, zValue);
        EjeZ->setValue(zValue);
        EjeZ->notify();
        
        delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms

        Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  "); 
        Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
        Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
        delay(500);
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        digitalWrite(pin_dos, LOW);
        pServer->startAdvertising(); // restart advertising
        Serial.println("El dispositivo ha sido desconectado");
        oldDeviceConnected = deviceConnected;
        
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        digitalWrite(pin_dos, HIGH);
        Serial.println("El dispositivo ha sido conectado");

        
        
        oldDeviceConnected = deviceConnected;
        
    }
    
    
  
  
  
}