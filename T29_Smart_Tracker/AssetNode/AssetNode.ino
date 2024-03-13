#include "BLEDevice.h"
#include <M5StickCPlus.h>

#define SERVICE_UUID "01234567-0123-4567-89ab-0123456789ab"
#define bleServerName "AssetTrackingSystem"

static BLEUUID positionCharacteristicUUID("01234567-0123-4567-89ab-0123456789cd");

static boolean doConnect = false;
static boolean connected = false;

static BLEAddress *pServerAddress;
static BLERemoteCharacteristic* positionCharacteristic;

static BLEClient* pClient;

const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

char* positionStr;
boolean newPosition = false;

int cornerNodeRSSI = 0; // Variable to store corner node RSSI

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == bleServerName) {
      advertisedDevice.getScan()->stop();
      pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      doConnect = true;
      Serial.println("Device found. Connecting!");
    }
    else
      Serial.print(".");
  }
};

static void positionNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  positionStr = (char*)pData;
  newPosition = true;
}

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.printf("Asset Node", 0);

  BLEDevice::init("");
 
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
}

void printReadings() {
  M5.Lcd.setCursor(0, 20, 2);
  M5.Lcd.print("Position = ");
  M5.Lcd.print(positionStr);

  // Print corner node RSSI
  M5.Lcd.setCursor(0, 40, 2);
  M5.Lcd.print("Corner Node RSSI = ");
  M5.Lcd.print(cornerNodeRSSI);
}

bool connectToServer(BLEAddress pAddress) {
  pClient = BLEDevice::createClient();
 
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");
 
  BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID");
    return (false);
  }
 
  positionCharacteristic = pRemoteService->getCharacteristic(positionCharacteristicUUID);

  if (positionCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    return false;
  }
  Serial.println(" - Found our characteristics");
 
  positionCharacteristic->registerForNotify(positionNotifyCallback);

  return true;
}

void loop() {
  if (doConnect == true && !connected) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("Connected to the BLE Server.");
      // Activate the Notify property of each Characteristic
      positionCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      connected = true;
    } else {
      Serial.println("Failed to connect to the server; Restart device to scan for nearby BLE server again.");
    }
    doConnect = false;
  }

  // Check if new position data is available
  if (newPosition) {
      newPosition = false;
      cornerNodeRSSI = pClient->getRssi();
      printReadings();
  }
    
  delay(1000);  // Delay one second between loops.
}
