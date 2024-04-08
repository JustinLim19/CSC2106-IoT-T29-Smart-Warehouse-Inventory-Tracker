#include "NimBLEDevice.h"
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SERVICE_UUID "01234567-0123-4567-89ab-0123456789ab"
#define CHARACTERISTIC_UUID "01234567-0123-4567-89ab-0123456789cd"
#define MAX_NODES 4

/* Put your SSID & Password */
const char* ssid = "konek";    // Change to your WiFi SSID
const char* password = "gayboy123";  // Change to your WiFi password
const char* serverUrl = "http://192.168.156.248:3000/json_endpoint"; // Change to your WiFi's IPv4 address

const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

static NimBLEUUID nodeCharacteristicUUID(CHARACTERISTIC_UUID);
static NimBLEUUID serviceUUID(SERVICE_UUID);

static NimBLEAdvertisedDevice* pAdvertisedDevices[MAX_NODES];
static NimBLERemoteCharacteristic* nodeCharacteristic;
static NimBLEClient* pClient;

// Define the coordinates of the corner nodes
double xCornerNode[] = {0.0, 6.0, 0.0, 0.0}; // Example x coordinates of corner nodes
double yCornerNode[] = {0.0, 0.0, 6.0, 0.0}; // Example y coordinates of corner nodes
double zCornerNode[] = {0.0, 0.0, 0.0, 6.0}; // Example z coordinates of corner nodes

int cornerNodeRssiArr[MAX_NODES]; // RSSI values from corner nodes
double distancesCornerNodes[MAX_NODES]; // Distances from corner nodes to asset node

// Variables to store estimated node of asset node
double xAssetNode = 0.0;
double yAssetNode = 0.0;
double zAssetNode = 0.0;

// Function prototypes
double rssiToDistance(int rssi);
void performTrilateration();
bool getNodeAddresses(NimBLEScan* pBLEScan);
void sendMessage();
void printReadings();
bool connectToServer(NimBLEAdvertisedDevice* pDevice, int index);

class MyClientCallback : public NimBLEClientCallbacks {
  void onConnect(NimBLEClient* pclient) {
    Serial.println("onConnect");
  }

  void onDisconnect(NimBLEClient* pclient) {
    Serial.println("onDisconnect");
  }
};

class AdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        if (advertisedDevice->isAdvertisingService(NimBLEUUID(SERVICE_UUID))) {
            Serial.println("Found our service");

            if (advertisedDevice->getName().compare("CornerNode1") == 0) {
              Serial.println("Found CornerNode1");
              pAdvertisedDevices[0] = advertisedDevice;
            }    
            else if (advertisedDevice->getName().compare("CornerNode2") == 0) {
              Serial.println("Found CornerNode2");
              pAdvertisedDevices[1] = advertisedDevice;
            }
            else if (advertisedDevice->getName().compare("CornerNode3") == 0) {
              Serial.println("Found CornerNode3");
              pAdvertisedDevices[2] = advertisedDevice;
            }
            else if (advertisedDevice->getName().compare("CornerNode4") == 0) {
              Serial.println("Found CornerNode4");
              pAdvertisedDevices[3] = advertisedDevice;
            } 
            else {
              Serial.println("Did not find corner node");
            }
        }
    };
};

static void nodeNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  // Handle notification callback if needed
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");

  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.printf("Asset Node", 0);

  NimBLEDevice::init("AssetNode");

  NimBLEScan* pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

bool connectToServer(NimBLEAdvertisedDevice* pDevice, int index) {
  Serial.print("Connecting to "); Serial.println(pDevice->getName().c_str());

  pClient = NimBLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());
 
  unsigned long startTime = millis(); // Record the start time

  // Attempt to connect to the server
  while (!pClient->connect(pDevice)) {
    // Check if timeout has occurred
    if (millis() - startTime > 5000) {
      Serial.println("Failed to connect, retrying...");
      NimBLEDevice::deleteClient(pClient);
      pClient = NimBLEDevice::createClient();
      startTime = millis();
    }
  }
  Serial.println(" - Connected to server");

  cornerNodeRssiArr[index] = pClient->getRssi();
 
  NimBLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.println("Failed to find our service UUID");
    return false;
  }
  Serial.println(" - Found our service");

  nodeCharacteristic = pRemoteService->getCharacteristic(nodeCharacteristicUUID);
  if (nodeCharacteristic == nullptr) {
    Serial.println("Failed to find our characteristic UUID");
    return false;
  }
  Serial.println(" - Found our characteristics");
  nodeCharacteristic->registerForNotify(nodeNotifyCallback);

  NimBLEDevice::deleteClient(pClient);
  return true;
}

double rssiToDistance(int rssi) {
  if (rssi == 0) {
    Serial.println("Error: RSSI value is zero (or near zero)");
    return 0;
  }
  // Implement path loss model here
  // distance = 10^((RSSI - A) / (10 * n)), where A and n are constants
  double A = -70; // RSSI from 1m from corner node
  double n = 2; // Example constant
  double dist = pow(10, (A - rssi) / (10 * n));

  Serial.print("Distance for "); Serial.print(rssi); Serial.print(": "); Serial.println(dist);

  return dist;
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

void sendMessage() {
  DynamicJsonDocument jsonDoc(200);
  jsonDoc["name"] = "AssetNode";
  jsonDoc["xcoord"] = int(xAssetNode);
  jsonDoc["ycoord"] = int(yAssetNode);
  jsonDoc["zcoord"] = int(zAssetNode);

  // Convert JSON document to string
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Send JSON data to server
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(jsonString);
  if (httpResponseCode > 0) {
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
  } else {
    Serial.printf("Error code: %d\n", httpResponseCode);
  }
  http.end();
}

void printReadings() {
  M5.Lcd.setCursor(0, 20, 2);
  M5.Lcd.printf("Asset Location = (%.0f, %.0f, %.0f)", xAssetNode, yAssetNode, zAssetNode);
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

void loop() {
  while (pAdvertisedDevices[0] == nullptr || pAdvertisedDevices[1] == nullptr || pAdvertisedDevices[2] == nullptr || pAdvertisedDevices[3] == nullptr) {
    NimBLEDevice::getScan()->start(5, false);
    Serial.println("Missing corner node(s)");
    delay(1000);
  }

  Serial.println("Found all corner nodes");
  NimBLEDevice::getScan()->stop();

  for (int i = 0; i < MAX_NODES; i++) {
    if (!connectToServer(pAdvertisedDevices[i], i)) {
      Serial.println("Failed to connect to the server.");
      NimBLEDevice::deleteClient(pClient);
      i = i; // Retry connection
    }
  }

  performTrilateration();
  sendMessage();
  printReadings();

  delay(1000);
}