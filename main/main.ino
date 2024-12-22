/*
   This example code sets GPIO16 and GPIO17 on an ESP32 as output pins.
   They will turn on (HIGH) together for 2 seconds and then off (LOW) for 2 seconds, repeatedly.
   This creates a 4-second cycle with a 50% duty cycle: 2 seconds on, 2 seconds off.

   The code also prints the current state of the pins to the Serial Monitor at 115200 baud.
*/

// Constants
const unsigned long interval = 100; // Interval in milliseconds

// Variables
unsigned long previousMillis = 0; // Stores the last time the function was called

void setup() {
  // Initialize serial communication at 115200 baud
  // [UNCHANGED] Initialize serial communication for debugging
  Serial.begin(115200);

  // [UNCHANGED] Initialize temperature sensor and halt if initialization fails
  if (initTemperatureSensor() != 0) {
    while (1); // [UNCHANGED] Enter infinite loop to prevent undefined behavior
  }

  // [UNCHANGED] Initialize distance sensor and halt if initialization fails
  if (initDistanceSensor() != 0) {
    while (1); // [UNCHANGED] Enter infinite loop to prevent undefined behavior
  }

  // [UNCHANGED] Initialize the limit switch and output signals
  InitLimitSwitch();
  InitOutput();
}

void loop() {
  unsigned long currentMillis = millis(); // [UPDATED] Use millis() for non-blocking timing

  // [UPDATED] Replaced delay with millis-based non-blocking timing to improve responsiveness
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Update the last execution time
    ControlLoop(); // [UNCHANGED] Call the periodic function to execute main logic
  }
}
