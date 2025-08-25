
const uint8_t BUTTON_PIN = 2;   // Pushbutton connected here
const uint8_t SENSOR_PIN = 3;   // PIR sensor output connected here
const uint8_t LED1_PIN   = 5;   // LED1 connected here
const uint8_t LED2_PIN   = 6;   // LED2 connected here


volatile bool buttonFlag = false;
volatile bool sensorFlag = false;

// These variables are used for debouncing 
unsigned long lastButtonMillis = 0;
unsigned long lastSensorMillis = 0;
const unsigned long DEBOUNCE_MS = 200; // 200ms = 0.2 second debounce window

// Variables to remember the ON/OFF state of LEDs
bool led1State = false;
bool led2State = false;

void setup() {
  Serial.begin(115200);              // Start Serial Monitor at 115200 baud
  while (!Serial) { }              

  // Configure LED pins as outputs
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);       // Start with LEDs OFF
  digitalWrite(LED2_PIN, LOW);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(SENSOR_PIN, INPUT);

  // Attach interrupts:
  // When button goes from HIGH to LOW (pressed), call buttonISR()
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  // When sensor goes from LOW to HIGH (motion detected), call sensorISR()
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), sensorISR, RISING);

  Serial.println("Task4.2C initialized. Waiting for button or sensor events...");
}

void loop() {

  if (buttonFlag) {                        // If button ISR has set the flag
    unsigned long now = millis();          
    if (now - lastButtonMillis >= DEBOUNCE_MS) {  // Check debounce
      lastButtonMillis = now;              // Update last valid press time
      led1State = !led1State;             
      digitalWrite(LED1_PIN, led1State ? HIGH : LOW);

      Serial.print("Button interrupt -> LED1 is now: ");
      Serial.println(led1State ? "ON" : "OFF");
    }
    buttonFlag = false;                    
  }

  // --- Handle Sensor Events ---
  if (sensorFlag) {                        
    unsigned long now = millis();
    if (now - lastSensorMillis >= DEBOUNCE_MS) {  // Check debounce
      lastSensorMillis = now;
      led2State = !led2State;              // Toggle LED2 state
      digitalWrite(LED2_PIN, led2State ? HIGH : LOW);

      Serial.print("Sensor interrupt -> LED2 is now: ");
      Serial.println(led2State ? "ON" : "OFF");
    }
    sensorFlag = false;                    // Clear the flag after handling
  }

  delay(10);  // Small delay to avoid busy looping
}


void buttonISR() {
  buttonFlag = true;  // Telling the main loop that the button was pressed.
}

void sensorISR() {
  sensorFlag = true;  // Telling the main loop that the sensor was triggered.
}