#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

/* Put your SSID & Password */
const char* ssid = "Askey5100-ED48";
const char* password = "HUJQKQ6v6L";
const char* serverUrl = "http://192.168.1.1:3000/json_endpoint";

int nodeValue = 1;
int xcoordValue = 0;
int ycoordValue = 0;
int zcoordValue = 1;


void setup() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");

}



void sendMessage(){

  // Sample JSON data
  DynamicJsonDocument jsonDoc(200);
  jsonDoc["name"] = nodeValue;
  jsonDoc["xcoord"] = xcoordValue;
  jsonDoc["ycoord"] = ycoordValue;
  jsonDoc["zcoord"] = zcoordValue;


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

void loop() {
  sendMessage();
  xcoordValue++;
  ycoordValue++;
  zcoordValue++;
  nodeValue++;
          delay(2000);
}
