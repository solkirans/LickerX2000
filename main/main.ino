/*
   This example code sets GPIO16 and GPIO17 on an ESP32 as output pins.
   They will turn on (HIGH) together for 2 seconds and then off (LOW) for 2 seconds, repeatedly.
   This creates a 4-second cycle with a 50% duty cycle: 2 seconds on, 2 seconds off.

   The code also prints the current state of the pins to the Serial Monitor at 115200 baud.
*/
#include "VL53L0XOccupancy.h"


// Constants
const unsigned long measurement_interval = 50; // Interval in milliseconds
const unsigned long control_interval = 500; // Interval in milliseconds
// Variables
unsigned long previousMillisMeasurement = 0; // Stores the last time the function was called
unsigned long previousMillisControl = 0; // Stores the last time the function was called

void setup() {
  // Initialize serial communication at 115200 baud
  // [UNCHANGED] Initialize serial communication for debugging
  Serial.begin(115200);
  delay(100); // Wait for 100 milliseconds
  InitOutput();
  delay(100); // Wait for 100 milliseconds
  if (I2C_init())
  {
    while(1);
  }
  delay(100); // Wait for 100 milliseconds

  // Initialize the sensor
  int return_VL53L0X_Init = VL53L0X_Init();
  if (return_VL53L0X_Init) {
    Serial.print("Initialization failed with error code: ");
    Serial.println(return_VL53L0X_Init);
    while (1); // Halt execution on error
  }
  // [UNCHANGED] Initialize temperature sensor and halt if initialization fails
  /*
  if (initTemperatureSensor() != 0) {
    while (1); // [UNCHANGED] Enter infinite loop to prevent undefined behavior
  }
  delay(100); // Wait for 100 milliseconds
  */

  // [UNCHANGED] Initialize distance sensor and halt if initialization fails
  
  // [UNCHANGED] Initialize the limit switch and output signals
  //InitLimitSwitch();
}

void loop() {
  unsigned long currentMillis = millis(); // [UPDATED] Use millis() for non-blocking timing

  // [UPDATED] Replaced delay with millis-based non-blocking timing to improve responsiveness
  if (currentMillis - previousMillisMeasurement >= measurement_interval) {
    previousMillisMeasurement = currentMillis; // Update the last execution time
      VL53L0X_ReadMeasurement();
      VL53L0X_GetAverage();
  }
  if (currentMillis - previousMillisControl >= control_interval) {
    previousMillisControl = currentMillis; // Update the last execution time
      VL53L0X_GetAverage();
  }
}
