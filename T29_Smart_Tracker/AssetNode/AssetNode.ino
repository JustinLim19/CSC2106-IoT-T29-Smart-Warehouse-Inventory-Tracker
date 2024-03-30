#include "BLEDevice.h"
#include <M5StickCPlus.h>

#define SERVICE_UUID "01234567-0123-4567-89ab-0123456789ab"
#define CHARACTERISTIC_UUID "01234567-0123-4567-89ab-0123456789cd"
#define MAX_NODES 4

const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

static BLEUUID nodeCharacteristicUUID(CHARACTERISTIC_UUID);
static BLEUUID serviceUUID(SERVICE_UUID);

static BLEAddress *pServerAddresses[MAX_NODES];
static BLERemoteCharacteristic* nodeCharacteristic;
static BLEClient* pClient;

// Define the coordinates of the corner nodes
double xCornerNode[] = {1.0, 6.0, 1.0, 1.0}; // Example x coordinates of corner nodes
double yCornerNode[] = {1.0, 1.0, 6.0, 1.0}; // Example y coordinates of corner nodes
double zCornerNode[] = {1.0, 1.0, 1.0, 6.0}; // Example z coordinates of corner nodes

int cornerNodeRssiArr[MAX_NODES]; // RSSI values from corner nodes
double distancesCornerNodes[MAX_NODES]; // Distances from corner nodes to asset node

// Variables to store estimated node of asset node
double xAssetNode = 0.0;
double yAssetNode = 0.0;
double zAssetNode = 0.0;

std::string cornerNodes[] = {
  "CornerNode1",
  "CornerNode2",
  "CornerNode3",
  "CornerNode4"
};

int cornerNodesDiscovered = 0;
static boolean doConnect = false;

// Function prototypes
double rssiToDistance(int rssi);
void performTrilateration();
bool getNodeAddresses(BLEScan* pBLEScan);
void printReadings();
bool connectToServer(BLEAddress pAddress, int index);

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // Nothing here for now
  }
};

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial.println("onConnect");
  }

  void onDisconnect(BLEClient* pclient) {
    Serial.println("onDisconnect");
  }
};

double rssiToDistance(int rssi) {
  if (rssi == 0) {
    Serial.println("Error: RSSI value is zero (or near zero)");
    return 0;
  }

  // Implement path loss model here
  // distance = 10^((RSSI - A) / (10 * n)), where A and n are constants
  double A = -50; // Example constant
  double n = 2.0; // Example constant
  return pow(10, (A - rssi) / (10 * n));
}

void performTrilateration() {
  double totalWeight = 0.0;
  double weightedX = 0.0;
  double weightedY = 0.0;
  double weightedZ = 0.0;
  double distance = 0.0;

  // Calculate total weight and weighted sum of coordinates
  for (int i = 0; i < MAX_NODES; i++) {
    // Avoid division by zero
    if ((distance = rssiToDistance(cornerNodeRssiArr[i])) != 0) {
      totalWeight += 1.0 / distance;
      weightedX += (1.0 / distance) * xCornerNode[i];
      weightedY += (1.0 / distance) * yCornerNode[i];
      weightedZ += (1.0 / distance) * zCornerNode[i];
    }
  }

  // Calculate interpolated coordinates
  if (totalWeight != 0) {
    xAssetNode = weightedX / totalWeight;
    yAssetNode = weightedY / totalWeight;
    zAssetNode = weightedZ / totalWeight;
  } else {
    // Handle the case when all distances are zero (or near zero)
    // Set asset node coordinates to an arbitrary value or handle it according to your application's requirements
    xAssetNode = 0.0;
    yAssetNode = 0.0;
    zAssetNode = 0.0;
    Serial.println("Error: All distances are zero (or near zero)");
  }
}

bool getNodeAddresses(BLEScan* pBLEScan) {
  Serial.println("Searching for corner nodes...");

  // Will keep scanning until all corner nodes found
  while (cornerNodesDiscovered < MAX_NODES) {
    BLEScanResults foundDevices = pBLEScan->start(5, false);

    for (int i = 0; i < foundDevices.getCount(); i++) {
      BLEAdvertisedDevice res = foundDevices.getDevice(i);
      // Check if found device is our corner node
      if (res.getName() == cornerNodes[cornerNodesDiscovered]) {
        cornerNodeRssiArr[cornerNodesDiscovered] = res.getRSSI();
        pServerAddresses[cornerNodesDiscovered] = new BLEAddress(res.getAddress());
        cornerNodesDiscovered++;
      }
    }
    pBLEScan->stop();
    pBLEScan->clearResults();
  }

  cornerNodesDiscovered = 0;
  Serial.println("Found all corner nodes!");

  return true;
}

static void nodeNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  // Handle notification callback if needed
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
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
  pBLEScan->setActiveScan(true);

  if (getNodeAddresses(pBLEScan)) {
    doConnect = true;
  }
}

void printReadings() {
  M5.Lcd.setCursor(0, 20, 2);
  M5.Lcd.printf("Asset Location = (%.2f, %.2f, %.2f)", xAssetNode, yAssetNode, zAssetNode);
  M5.Lcd.println();

  // Print each corner node RSSI
  M5.Lcd.setCursor(0, 40, 2);
  for (int i = 0; i < MAX_NODES; i++) {
    M5.Lcd.print("Corner Node ");
    M5.Lcd.print(i + 1);
    M5.Lcd.print(" RSSI = ");
    M5.Lcd.print(cornerNodeRssiArr[i]);
    M5.Lcd.println();
  }
}

void connectToServer(BLEAddress pAddress, int index) {
  Serial.print("Connecting to CornerNode"); Serial.println(index+1);

  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());
 
  unsigned long startTime = millis(); // Record the start time

  // Attempt to connect to the server
  while (!pClient->connect(pAddress)) {
    // Check if timeout has occurred
    if (millis() - startTime > 5000) {
      Serial.println("Failed to connect, retrying...");
      pClient->disconnect();
      delay(1000);
      pClient = BLEDevice::createClient();
      startTime = millis();
    }
  }
  Serial.println(" - Connected to server");

  cornerNodeRssiArr[index] = pClient->getRssi();
 
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.println("Failed to find our service UUID");
  }
  Serial.println(" - Found our service");

  nodeCharacteristic = pRemoteService->getCharacteristic(nodeCharacteristicUUID);
  if (nodeCharacteristic == nullptr) {
    Serial.println("Failed to find our characteristic UUID");
  }
  Serial.println(" - Found our characteristics");
  nodeCharacteristic->registerForNotify(nodeNotifyCallback);

  pClient->disconnect();
  delay(1000);
}

void loop() {
  // printMemoryUsage();
  if (doConnect) {
    // Connect to each corner node found
    for (int i = 0; i < MAX_NODES; i++) {
      connectToServer(*pServerAddresses[i], i);
      delay(1000);
    }
    doConnect = false;
  }
  else {
    performTrilateration();
    printReadings();
    doConnect = true;
  }
  delay(1000);
}