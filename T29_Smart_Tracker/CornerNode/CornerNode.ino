#include "NimBLEDevice.h"
// #include <BLEDevice.h>
// #include <BLEServer.h>
#include <M5StickCPlus.h>

#define SERVICE_UUID "01234567-0123-4567-89ab-0123456789ab"
#define CHARACTERISTIC_UUID "01234567-0123-4567-89ab-0123456789cd"

// When uploading to each device change accordingly
// #define bleServerName "CornerNode1" 
#define bleServerName "CornerNode2" 
// #define bleServerName "CornerNode3"
// #define bleServerName "CornerNode4"

// NimBLECharacteristic nodeCharacteristics("01234567-0123-4567-89ab-0123456789cd", NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
// NimBLEDescriptor nodeDescriptor(NimBLEUUID((uint16_t)0x2902));

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
bool deviceConnected = false;

class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer) {
    deviceConnected = true;

    Serial.println("MyServerCallbacks::Connected...");

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println(bleServerName);
    M5.Lcd.println("Connected to a client!");
  };
  void onDisconnect(NimBLEServer* pServer) {
    deviceConnected = false;

    Serial.println("MyServerCallbacks::Disconnected...");

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println(bleServerName);
    M5.Lcd.println("Waiting for client connection...");
  }
};

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.println(bleServerName);
  M5.Lcd.println("Waiting for client connection...");

  NimBLEDevice::init(bleServerName);
    
  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  NimBLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID);

  pService->start();
  pCharacteristic->setValue("Hello BLE");

  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID); 
  pAdvertising->start(); 

  // BLEDevice::init(bleServerName);

  // BLEServer *pServer = BLEDevice::createServer();
  // pServer->setCallbacks(new MyServerCallbacks());

  // BLEService *bleService = pServer->createService(SERVICE_UUID);

  // bleService->addCharacteristic(&nodeCharacteristics);
  // nodeCharacteristics.setValue("Node");
  // nodeCharacteristics.addDescriptor(&nodeDescriptor);

  // bleService->start();

  // BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  // pAdvertising->addServiceUUID(SERVICE_UUID);
  // pServer->getAdvertising()->start();
  Serial.println("Waiting for client connection to notify...");
}

void loop() {
  // BLEDevice::startAdvertising();
  NimBLEDevice::startAdvertising();
  // if (deviceConnected) {
  //   if ((millis() - lastTime) > timerDelay) {
  //     nodeCharacteristics.setValue(bleServerName);
  //     nodeCharacteristics.notify();
  //     lastTime = millis();
  //   }
  // }
  delay(100);
}
