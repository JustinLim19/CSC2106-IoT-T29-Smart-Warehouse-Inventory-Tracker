#include <BLEDevice.h>
#include <BLEServer.h>
#include <M5StickCPlus.h>

#define SERVICE_UUID "01234567-0123-4567-89ab-0123456789ab"
#define bleServerName "AssetTrackingSystem"

BLECharacteristic positionCharacteristics("01234567-0123-4567-89ab-0123456789cd", BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLEDescriptor positionDescriptor(BLEUUID((uint16_t)0x2902));

unsigned long lastTime = 0;
unsigned long timerDelay = 15000;
bool deviceConnected = false;
bool ledStatus = false;  // Initial LED status

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("MyServerCallbacks::Connected...");
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("MyServerCallbacks::Disconnected...");
  }
};

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.printf("BLE Server", 0);

  BLEDevice::init(bleServerName);

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *bleService = pServer->createService(SERVICE_UUID);

  bleService->addCharacteristic(&positionCharacteristics);
  positionCharacteristics.setValue("Node Position");
  positionCharacteristics.addDescriptor(&positionDescriptor);

  bleService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("Waiting for client connection to notify...");
}

void loop() {
  BLEDevice::startAdvertising();

  if (deviceConnected) {
    if ((millis() - lastTime) > timerDelay) {
      // Simulate changing position data
      static int xPos = 0;
      static int yPos = 0;
      static int zPos = 0;
      xPos++;
      yPos++;
      zPos++;
      String positionData = String(xPos) + ":" + String(yPos) + ":" + String(zPos);
      positionCharacteristics.setValue(positionData.c_str());
      positionCharacteristics.notify();
      
      // Display position data on the LCD
      M5.Lcd.fillScreen(TFT_BLACK);
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.printf("Position: %s", positionData.c_str());

      lastTime = millis();
    }
  }

  delay(100);  // Delay for stability
}
