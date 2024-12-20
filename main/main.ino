/*
   This example code sets GPIO16 and GPIO17 on an ESP32 as output pins.
   They will turn on (HIGH) together for 2 seconds and then off (LOW) for 2 seconds, repeatedly.
   This creates a 4-second cycle with a 50% duty cycle: 2 seconds on, 2 seconds off.

   The code also prints the current state of the pins to the Serial Monitor at 115200 baud.
*/

const int pin1 = 16;
const int pin2 = 17;

void setup() {
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);

  // Set pins as outputs
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);

  // Initial state is LOW
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
}

void loop() {
  // Turn both pins ON (HIGH)
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  Serial.print("Pin ");
  Serial.print(pin1);
  Serial.print(" and ");
  Serial.print(pin2);
  Serial.println(" is ON");
  
  // Keep them enabled for 2 seconds
  delay(2000);

  // Turn both pins OFF (LOW)
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  Serial.print("Pin ");
  Serial.print(pin1);
  Serial.print(" and  ");
  Serial.print(pin2);
  Serial.println(" is OFF");
  
  // Keep them disabled for 2 seconds
  delay(2000);

  // The loop will now repeat, maintaining a 4-second cycle (2s ON, 2s OFF).
}
