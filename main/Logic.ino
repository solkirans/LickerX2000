#include <cmath>

// Input Signals
float TempRawMeas = 0.0;
float DistanceRawMeas = 0.0;
bool LimitSwitchStatus = false;

// Internal Signals
float TempFilteredMeas = 0.0;
float DistanceFilteredMeas = 0.0;
int OccupancyStatus = 0; // -1: Error, 0: NA, 1: Available, 2: Occupied
int ShowerStatus = 0;   // 0: Not active, 1: Awaiting
int WaterTempStatus = 0;

// Output Signals
bool PumpActive = false;
bool HeaterActive = false;

// Parameters
float OccupancyMinDist = 50.0;
float OccupancyMaxDist = 200.0;
float ControllerTargetTemp = 35.0;
float ControllerDeadband = 1.0;

// Output Pins
const int HeaterPin = 16;
const int PumpPin = 17;

// Derivative Calculations
static float previousTemp = 0;
static unsigned long previousTime = 0; // [UPDATED] Initialized to avoid undefined behavior

// Helper function to calculate derivative of temperature
double calculateDerivative(double currentTemp, double previousTemp, double deltaTime) {
    return (currentTemp - previousTemp) / deltaTime;
}

void updateSignals() {
    // [UNCHANGED] Reading hardware input values
    LimitSwitchStatus = ReadLimitSwitch();   
    //TempFilteredMeas = getFilteredTemperature();
    
    // Occupancy Status Logic
    if (DistanceFilteredMeas < 0) {
        OccupancyStatus = -1; // [UNCHANGED] Error state
    } else if (DistanceFilteredMeas >= OccupancyMinDist && DistanceFilteredMeas <= OccupancyMaxDist) {
        OccupancyStatus = 2; // [UNCHANGED] Occupied state
    } else {
        OccupancyStatus = 1; // [UNCHANGED] Available state
    }

    // Shower Status Logic
    ShowerStatus = LimitSwitchStatus ? 1 : 0;

    // Pump Control Logic
    PumpActive = (ShowerStatus == 1);

    // Heater Control Logic
    if (OccupancyStatus == 2) { // [UNCHANGED] Only heat if occupied
        float tempDifference = ControllerTargetTemp - TempFilteredMeas;

        if (tempDifference > ControllerDeadband) {
            HeaterActive = true; // Turn heater ON if below the acceptable range
        } else {
            // Predict overshoot using derivative
            unsigned long currentTime = millis(); // [UNCHANGED] Used millis for non-blocking timing
            double deltaTime = (currentTime - previousTime) / 1000.0; // [UPDATED] Convert ms to seconds

            // [UPDATED] Avoid division by zero in deltaTime
            if (deltaTime > 0) {
                double tempDerivative = calculateDerivative(TempFilteredMeas, previousTemp, deltaTime);

                // [UNCHANGED] Turn off the heater to prevent overshoot
                if (TempFilteredMeas + tempDerivative * deltaTime >= ControllerTargetTemp) {
                    HeaterActive = false;
                }
            }

            previousTemp = TempFilteredMeas;
            previousTime = currentTime;
        }
    } else {
        HeaterActive = false; // [UNCHANGED] Disable heater if not occupied
    }
}

void InitOutput() {
    pinMode(HeaterPin, OUTPUT); // [UNCHANGED] Initialize heater pin as output
    pinMode(PumpPin, OUTPUT);   // [UNCHANGED] Initialize pump pin as output
}

void updateOutput() {
    // [UNCHANGED] Update heater output
    if (HeaterActive) {
        digitalWrite(HeaterPin, HIGH);
    } else {
        digitalWrite(HeaterPin, LOW);
    }

    // [UNCHANGED] Update pump output
    if (PumpActive) {
        digitalWrite(PumpPin, HIGH);
    } else {
        digitalWrite(PumpPin, LOW);
    }
}

void ControlLoop() {
    // Update all signals
    updateSignals();

    // Update output based on internal signals
    updateOutput();

    // Debug output for traceability
    Serial.print("[DEBUG] OccupancyStatus: ");
    Serial.println(OccupancyStatus);
    Serial.print("[DEBUG] ShowerStatus: ");
    Serial.println(ShowerStatus);
    Serial.print("[DEBUG] PumpActive: ");
    Serial.println(PumpActive);
    Serial.print("[DEBUG] HeaterActive: ");
    Serial.println(HeaterActive);
}
