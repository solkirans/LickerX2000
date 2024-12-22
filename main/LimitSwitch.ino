#include <Arduino.h>

#define LIMIT_SWITCH_PIN 5
const unsigned long DEBOUNCE_TIME_LOW = 1000; // 1 second for inactive state
const unsigned long DEBOUNCE_TIME_HIGH = 250; // 250 ms for active state

int lastSwitchState = HIGH; // Stores the last stable state of the switch
unsigned long lastDebounceTime = 0; // Tracks the last debounce time

void InitLimitSwitch() {
    pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP); // Configure limit switch pin with pull-up resistor
}

bool ReadLimitSwitch() { // [UNCHANGED] Original function structure retained
    // [UPDATED] Reading the current state of the limit switch
    int currentSwitchState = digitalRead(LIMIT_SWITCH_PIN);
    unsigned long currentTime = millis();

    // [UPDATED] Debounce logic to handle state transitions
    // Check if the state has changed, and reset the debounce timer if it has
    if (currentSwitchState != lastSwitchState) {
        lastDebounceTime = currentTime;
        lastSwitchState = currentSwitchState;
    }

    // [UPDATED] Stable LOW state (inactive) validation after debounce time
    if (currentSwitchState == LOW && (currentTime - lastDebounceTime) >= DEBOUNCE_TIME_LOW) {
        return false; // Limit switch is inactive
    }

    // [UPDATED] Stable HIGH state (active) validation after debounce time
    if (currentSwitchState == HIGH && (currentTime - lastDebounceTime) >= DEBOUNCE_TIME_HIGH) {
        return true; // Limit switch is active
    }

    // [UNCHANGED] No state change detected, retain the last known state
    return lastSwitchState == HIGH;
}