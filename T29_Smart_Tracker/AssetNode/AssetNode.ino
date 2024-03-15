#include "BLEDevice.h"
#include <M5StickCPlus.h>

#define SERVICE_UUID "01234567-0123-4567-89ab-0123456789ab"
#define bleServerName "AssetTrackingSystem"

#define MAX_NODES 3

static BLEUUID nodeCharacteristicUUID("01234567-0123-4567-89ab-0123456789cd");
static BLEUUID serviceUUID(SERVICE_UUID);

static boolean doConnect = false;
static boolean connected = false;

static BLEAddress *pServerAddress;
static BLERemoteCharacteristic* nodeCharacteristic;
static BLEAddress *pServerAddresses[MAX_NODES];

static BLEClient* pClient;
static BLEClient* pNodes[MAX_NODES];

const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

char* nodeStr;
boolean newNode = false;

// Known nodes (coordinates) of corner nodes
double xCornerNode1 = 0.0;
double yCornerNode1 = 0.0;
double xCornerNode2 = 5.0; // Example coordinates (adjust as per your setup)
double yCornerNode2 = 0.0;
double xCornerNode3 = 0.0;
double yCornerNode3 = 5.0;

int cornerNodeRssiArr[MAX_NODES] = { 0, 0, 0 }; // RSSI values from corner nodes
double distancesCornerNodes[MAX_NODES] = { 0.0, 0.0, 0.0 }; // Distances from corner nodes to asset node

// Variables to store estimated node of asset node
double xAssetNode = 0.0;
double yAssetNode = 0.0;

std::string cornerNodes[] = {
  "CornerNode1",
  "CornerNode2",
  "CornerNode3"
};

int cornerNodesDiscovered = 0;
bool gotAllRssi = false;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // Nothing here for now
  }
};

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    connected = true;
    Serial.print("Connected to ");
    Serial.print(pClient->getConnId());
    Serial.println();
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    // doConnect = true;
    Serial.println("onDisconnect");
  }
};

double rssiToDistance(int rssi) {
    // Implement path loss model here
    // Example: distance = 10^((RSSI - A) / (10 * n)), where A and n are constants
    // You'll need to adjust the constants A and n based on your environment and hardware characteristics
    // This is just a placeholder, you should replace it with a proper path loss model
    double A = 50; // Example constant
    double n = 2.0; // Example constant
    return pow(10, (A - rssi) / (10 * n));
}

void performTrilateration() {
    // Calculate distances from RSSI values using path loss model
    for (int i = 0; i < MAX_NODES; i++) {
      distancesCornerNodes[i] = rssiToDistance(cornerNodeRssiArr[i]);
    }

    // Trilateration algorithm
    // Solve equations based on distances and known nodes of corner nodes
    // to estimate the coordinates of the asset node
    // Example algorithm:
    // Calculate intermediate values for trilateration equations
    double A = 2 * (xCornerNode2 - xCornerNode1);
    double B = 2 * (yCornerNode2 - yCornerNode1);
    double C = 2 * (xCornerNode3 - xCornerNode1);
    double D = 2 * (yCornerNode3 - yCornerNode1);

    double E = pow(distancesCornerNodes[0], 2) - pow(distancesCornerNodes[1], 2) - pow(xCornerNode1, 2) + pow(xCornerNode2, 2) - pow(yCornerNode1, 2) + pow(yCornerNode2, 2);
    double F = pow(distancesCornerNodes[1], 2) - pow(distancesCornerNodes[3], 2) - pow(xCornerNode1, 2) + pow(xCornerNode3, 2) - pow(yCornerNode1, 2) + pow(yCornerNode3, 2);

    // Calculate asset node's coordinates
    xAssetNode = (E - F * B / D) / (A - C * B / D);
    yAssetNode = (E - A * xAssetNode) / B;
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
  }

  gotAllRssi = true;
  cornerNodesDiscovered = 0;
  Serial.println("Found all corner nodes!");
  return true;
}

static void nodeNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  nodeStr = (char*)pData;
  Serial.println(nodeStr);
  newNode = true;
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
  M5.Lcd.printf("Asset Location = ( %f, %f )", xAssetNode, yAssetNode);
  M5.Lcd.println();

  // Print each corner node RSSI
  M5.Lcd.setCursor(0, 40, 2);
  for (int i = 0; i < MAX_NODES; i++) {
    M5.Lcd.print("Corner Node ");
    M5.Lcd.print(i + 1);
    M5.Lcd.print(" = ");
    M5.Lcd.print(cornerNodeRssiArr[i]);
    M5.Lcd.println();
  }
}

bool connectToServer(BLEAddress pAddress, int index) {
  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());
 
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");
 
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID");
    return (false);
  }

  nodeCharacteristic = pRemoteService->getCharacteristic(nodeCharacteristicUUID);
  if (nodeCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    return false;
  }
  Serial.println(" - Found our characteristics");
  nodeCharacteristic->registerForNotify(nodeNotifyCallback);

  pNodes[index] = pClient;

  return true;
}

void loop() {
  if (doConnect) {
    // Connect to each corner node found
    for (int i = 0; i < MAX_NODES; i++) {
      if (connectToServer(*pServerAddresses[i], i)) {
        Serial.print("Connected to the BLE Server: "); Serial.println(pServerAddresses[i]->toString().c_str());
        // Activate the Notify property of each Characteristic
        nodeCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      } else {
        Serial.println("Failed to connect to the server; Restart device to scan for nearby BLE server again.");
      }
    }
    connected = true;
    doConnect = false;
  } 
  else {
    if (connected) {
      if (gotAllRssi) {
        performTrilateration();
        printReadings();
        gotAllRssi = false;
      } 
      else {
        // After getting RSSI and performing trilateration once,
        // disconnet all currently connected nodes and connect again
        // so RSSI can be obtained again
        for (int i = 0; i < MAX_NODES; i++) {
          pNodes[i]->disconnect();
          Serial.print("Disonnected to the BLE Server: "); Serial.println(pServerAddresses[i]->toString().c_str());
        }
        connected = false;
      }
    } 
    else {
      BLEScan* pBLEScan = BLEDevice::getScan();
      pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
      pBLEScan->setInterval(100);
      pBLEScan->setWindow(99);
      pBLEScan->setActiveScan(true);

      if (getNodeAddresses(pBLEScan)) {
        doConnect = true;
      }
    }
  }
    
  delay(1000);  // Delay one second between loops.
}