#include <Arduino.h>


// Constants and Paremeters
#define LIMIT_SWITCH_PIN 5
const unsigned long DEBOUNCE_TIME_LOW = 250;  // 0.25 second to transition to low state
const unsigned long DEBOUNCE_TIME_HIGH = 1000; // 1 second to transition to high state



bool lastSwitchState = false; // Stores the last stable state of the switch
unsigned long lastDebounceTime = 0; // Tracks the last debounce time

void InitLimitSwitch() {
    pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP); // Configure limit switch pin with pull-up resistor
}

bool ReadLimitSwitch() { // [UNCHANGED] Original function structure retained
    // [UPDATED] Reading the current state of the limit switch
    bool currentSwitchState = digitalRead(LIMIT_SWITCH_PIN);
    unsigned long currentTime = millis();

    // [UPDATED] Debounce logic to handle state transitions
    // Check if the state has changed, and reset the debounce timer if it has
    if (currentSwitchState != lastSwitchState) {
        lastDebounceTime = currentTime;
        lastSwitchState = currentSwitchState;
    }

    // [UPDATED] Stable LOW state (inactive) validation after debounce time
    if (currentSwitchState == false && (currentTime - lastDebounceTime) >= DEBOUNCE_TIME_LOW) {
        return false; // Limit switch is inactive
    }

    // [UPDATED] Stable HIGH state (active) validation after debounce time
    if (currentSwitchState == true && (currentTime - lastDebounceTime) >= DEBOUNCE_TIME_HIGH) {
        return true; // Limit switch is active
    }

    // [UNCHANGED] No state change detected, retain the last known state
    return lastSwitchState == false;
}