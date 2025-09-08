#include <SAMDTimerInterrupt.h>
#include "DHT.h"

//  Pin Definitions 
#define BUTTON_PIN 2
#define PIR_PIN    3
#define DHT_PIN    4
#define LED1_PIN   8
#define LED2_PIN   9
#define LED3_PIN   10

// DHT Setup 
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

// Timer Setup 
SAMDTimer ITimer0(TIMER_TC3);

//  Volatile Flags 
volatile bool led1ToggleFlag = false;
volatile bool led2ToggleFlag = false;
volatile bool timerFlag      = false;

//  LED States 
bool led1State = LOW;
bool led2State = LOW;
bool led3State = LOW;

//  Debounce 
unsigned long lastButtonTime = 0;
unsigned long lastPirTime    = 0;
unsigned long lastDHTRead    = 0; 

//  ISRs 
void handleButton() {
  led1ToggleFlag = true;
}

void handlePIR() {
  led2ToggleFlag = true;
}

void TimerHandler() {
  timerFlag = true;
}

//  Setup 
void setup() {
  Serial.begin(115200);
  delay(2000);   

  Serial.println("Starting program with SAMDTimerInterrupt (1s timer, DHT every 2s)...");

  // LED pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  // Input pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);   
  pinMode(PIR_PIN, INPUT);             

  // Init DHT
  dht.begin();
  delay(2000);  

  // Attach external interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), handlePIR, RISING);

  // Attach timer interrupt at 1 Hz (once per second)
  if (ITimer0.attachInterruptInterval(1000000, TimerHandler)) {  
    Serial.println("Timer initialized successfully (1s).");
  } else {
    Serial.println("Timer initialization failed!");
  }
}

// Loop 
void loop() {
  unsigned long now = millis();

  // Handle button 
  if (led1ToggleFlag && (now - lastButtonTime > 200)) {
    led1ToggleFlag = false;
    lastButtonTime = now;

    led1State = !led1State;
    digitalWrite(LED1_PIN, led1State);
    Serial.println("Button pressed → LED1 toggled");
  }

  // Handle PIR (with debounce)
  if (led2ToggleFlag && (now - lastPirTime > 500)) {
    led2ToggleFlag = false;
    lastPirTime = now;

    led2State = !led2State;
    digitalWrite(LED2_PIN, led2State);
    Serial.println("PIR motion detected → LED2 toggled");
  }

  // Handle Timer ISR flag
  if (timerFlag) {
    timerFlag = false;

    // Toggle LED3 every second
    led3State = !led3State;
    digitalWrite(LED3_PIN, led3State);

    // Read DHT every 2 seconds
    if (now - lastDHTRead >= 2000) {
      lastDHTRead = now;

      float temp = dht.readTemperature();
      float hum  = dht.readHumidity();

      if (isnan(temp) || isnan(hum)) {
        Serial.println("Failed to read DHT22!");
      } else {
        Serial.print("Timer → LED3 toggled | Temp: ");
        Serial.print(temp);
        Serial.print(" °C, Hum: ");
        Serial.print(hum);
        Serial.println(" %");
      }
    }
  }
}