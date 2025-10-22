#include <Arduino.h>
#include <TinyGPS++.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <wiring_private.h>

// WiFi & MQTT credentials 
const char* ssid     = "Dhruv";
const char* password = "Dhruv@70277";
const char* mqtt_server = "192.168.57.18";  // Your Raspberry Pi's IP

//  GPS Pins 
#define GPS_RX_PIN 5
#define GPS_TX_PIN 6
#define GPS_BAUD   9600

// Ultrasonic sensor pins 
#define TRIG_PIN 9
#define ECHO_PIN 10
#define PROXIMITY_THRESHOLD_CM 30  // Alert if distance less than 30cm

// create GPS object 
TinyGPSPlus gps;

// Setup extra UART for GPS (using SERCOM on Arduino Nano 33 IoT)
Uart SerialGPS(&sercom0, GPS_RX_PIN, GPS_TX_PIN, SERCOM_RX_PAD_1, UART_TX_PAD_0);

void SERCOM0_Handler() {
  SerialGPS.IrqHandler();
}

// WiFi and MQTT clients 
WiFiClient espClient;
PubSubClient client(espClient);

// Function ;-  Connect to WiFi
void setupWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);  

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
// printing the details 
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}
// connecting to mqtt broker
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("CarClient001")) {
      Serial.println("connected");
      client.publish("car/001/hello", "Car unit online");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(". Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// Ultrasonic sensor distance function
long getUltrasonicDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);  // Timeout after 25ms
  if (duration == 0) return -1; // no echo received

  long distance = duration * 0.034 / 2;  // convert to cm
  return distance;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Starting GPS + Ultrasonic + MQTT system");

  // Setup GPS pins for SERCOM UART
  pinPeripheral(GPS_RX_PIN, PIO_SERCOM);
  pinPeripheral(GPS_TX_PIN, PIO_SERCOM);
  SerialGPS.begin(GPS_BAUD);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  setupWiFi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Read and parse GPS data
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
  }

  // Proximity Detection 
  long distance = getUltrasonicDistanceCM();

  if (distance > 0 && distance <= PROXIMITY_THRESHOLD_CM) {
    Serial.print("Object too close! Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    char proximityMsg[64];
    snprintf(proximityMsg, sizeof(proximityMsg),
             "{\"distance_cm\":%ld,\"alert\":\"object_too_close\"}", distance);
    client.publish("car/001/proximity", proximityMsg);

  } else if (distance > 0) {
    Serial.print(" Safe distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.println("⚠️ Ultrasonic sensor timeout/no echo");
  }

  // --- Publish GPS Data ---
  if (gps.location.isUpdated()) {
    char gpsMsg[128];
    snprintf(gpsMsg, sizeof(gpsMsg),
             "{\"lat\":%.6f,\"lng\":%.6f,\"altitude\":%.2f,\"sats\":%d}",
             gps.location.lat(),
             gps.location.lng(),
             gps.altitude.meters(),
             gps.satellites.value()
    );
    Serial.print("📡 GPS: ");
    Serial.println(gpsMsg);

    client.publish("car/001/gps", gpsMsg);
  }

  delay(1000);  // Main loop delay
}
