#include <WiFiNINA.h>          
#include <ArduinoMqttClient.h>  

char ssid[] = "HP7109";       
char pass[] = "Dhruv";   

const char broker[] = "850095e0b2bd406a85788734b3d060a5.s1.eu.hivemq.cloud";
int port = 8883;  

const char topic[] = "SIT210/wave";

const char mqttUser[] = "hivemq.webclient.1756138926540";
const char mqttPass[] = "43*KaOH9WdAp0%m!w.Dj";

WiFiSSLClient wifiClient;
MqttClient mqttClient(wifiClient);

// ---------------- Hardware Pins ----------------
// Pins for ultrasonic sensor
const int trigPin = 3;
const int echoPin = 2;

// Pin for LED
const int ledPin  = 7;

// Variables for ultrasonic sensor distance measurement
long duration;
int distance;

// ---------------- Setup Function ----------------
// Runs once at the beginning when the board is powered on or reset
void setup() {
  Serial.begin(9600);  // Start serial communication for monitoring

  // Set pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Connect to WiFi
  connectWiFi();

  // Set MQTT username and password for HiveMQ Cloud
  mqttClient.setUsernamePassword(mqttUser, mqttPass);

  // Connect to HiveMQ Cloud broker
  Serial.print("Connecting to HiveMQ Cloud...");
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed. Error code = ");
    Serial.println(mqttClient.connectError());
    while (1); // Stop program if connection fails
  }
  Serial.println("connected!");

  // Subscribe to topic
  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(topic);
  Serial.println("Subscribed to topic: SIT210/wave");
}

// ---------------- Loop Function ----------------
// Runs repeatedly while the board is powered on
void loop() {
  // Check for incoming MQTT messages
  mqttClient.poll();

  distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > 0 && distance < 20) {
    Serial.println("Wave detected. Publishing message...");
    mqttClient.beginMessage(topic);
    mqttClient.print("wave:Dhruv"); 
    mqttClient.endMessage();
    delay(2000); 
  }
}

int getDistance() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);


  return duration * 0.034 / 2;
}

void onMqttMessage(int messageSize) {
  String msg = "";

  // Read all characters in the message
  while (mqttClient.available()) {
    msg += (char)mqttClient.read();
  }

  Serial.println("Message received: " + msg);

  if (msg.startsWith("wave")) {
    blinkLED(3, 300);
  }

  else if (msg.startsWith("pat")) {
    blinkLED(4, 800);
  }
}

void blinkLED(int times, int delayTime) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(delayTime);
    digitalWrite(ledPin, LOW);
    delay(delayTime);
  }
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to WiFi.");
}