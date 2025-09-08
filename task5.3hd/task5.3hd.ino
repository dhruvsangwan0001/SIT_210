#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

// CHANGE THESE TO YOUR WIFI
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// CHANGE THIS TO YOUR FIREBASE
const char* firebase_host = "sit210-825ad-default-rtdb.firebaseio.com";
const int firebase_port = 443;

// LED Pins
const int RED = 2;
const int GREEN = 3;
const int BLUE = 4;

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, firebase_host, firebase_port);

void setup() {
  Serial.begin(9600);
  
  // Setup LEDs
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  
  // Connect WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected!");
}

void loop() {
  // Check Firebase every 2 seconds
  client.get("/leds.json");
  String response = client.responseBody();
  
  // Control LEDs based on response
  if (response.indexOf("\"red\":true") > -1) {
    digitalWrite(RED, HIGH);
  } else {
    digitalWrite(RED, LOW);
  }
  
  if (response.indexOf("\"green\":true") > -1) {
    digitalWrite(GREEN, HIGH);
  } else {
    digitalWrite(GREEN, LOW);
  }
  
  if (response.indexOf("\"blue\":true") > -1) {
    digitalWrite(BLUE, HIGH);
  } else {
    digitalWrite(BLUE, LOW);
  }
  
  delay(2000);
}