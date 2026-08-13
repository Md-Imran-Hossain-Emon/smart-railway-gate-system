/*
  ============================================================================
   Smart Railway Gate System
   Design and Implementation of an Intelligent Autonomous Railway Crossing
   Safety Framework with Multi-Sensor Redundancy

   Board   : ESP32-WROOM-32
   Author  : Md. Imran Hossain Emon
   Course  : EEE 3100 - Electronics Shop Practice, Dept. of EEE, RUET

   Description
   -----------
   Four heterogeneous sensors (2x HC-SR04 ultrasonic, 2x IR) watch the left
   and right approaches of a level crossing. A 3-out-of-4 majority voting
   algorithm with a 1000 ms temporal confirmation window detects and latches
   individual sensor faults without ever leaving the crossing unprotected.
   Live telemetry (distances, detection states, gate position, fault flags)
   streams over Bluetooth Serial at 5 Hz (200 ms) to a companion mobile app.
  ============================================================================
*/

#include <BluetoothSerial.h>
#include <ESP32Servo.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Enable it in "Tools -> Partition Scheme".
#endif

// ---------------------------------------------------------------------------
// Pin Map
// ---------------------------------------------------------------------------
const uint8_t trigPin1 = 5;    // Left  ultrasonic - trigger
const uint8_t trigPin2 = 19;   // Right ultrasonic - trigger
const uint8_t echoPin1 = 34;   // Left  ultrasonic - echo (input-only, safe)
const uint8_t echoPin2 = 35;   // Right ultrasonic - echo (input-only, safe)

const uint8_t irPin1   = 22;   // Left  IR sensor (digital)
const uint8_t irPin2   = 36;   // Right IR sensor (digital, input-only)

const uint8_t servoPin1 = 32;  // Left  barrier servo
const uint8_t servoPin2 = 33;  // Right barrier servo

const uint8_t buzzerPin  = 25; // Piezo buzzer
const uint8_t redLedPin  = 26; // Danger / gate-closed indicator
const uint8_t greenLedPin= 27; // Safe   / gate-open indicator

// ---------------------------------------------------------------------------
// System Parameters (from Table I of the project report)
// ---------------------------------------------------------------------------
const float    DISTANCE_THRESHOLD_CM = 10.0;   // Ultrasonic trigger distance
const uint32_t FAULT_CONFIRM_MS      = 1000;   // Fault latch confirmation window
const uint32_t TX_INTERVAL_MS        = 200;    // Telemetry interval  -> 5 Hz
const uint32_t PULSE_TIMEOUT_US      = 30000;  // pulseIn() upper bound
const uint32_t LOOP_DELAY_MS         = 20;     // Main loop tick

const uint8_t GATE_OPEN_ANGLE   = 0;
const uint8_t GATE_CLOSED_ANGLE = 90;

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
BluetoothSerial SerialBT;
Servo gateServoL, gateServoR;

// Sensor indices: 0 = US1(left US), 1 = IR1(left IR), 2 = US2(right US), 3 = IR2(right IR)
bool     detected[4]     = {false, false, false, false};
bool     faultLatched[4] = {false, false, false, false};
uint32_t faultTimerStart[4] = {0, 0, 0, 0};
bool     faultTimerRunning[4] = {false, false, false, false};

float distanceL = -1, distanceR = -1;

uint32_t lastTelemetry = 0;
uint32_t lastLoop      = 0;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
float readUltrasonicCM(uint8_t trigPin, uint8_t echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, PULSE_TIMEOUT_US);
  if (duration == 0) return -1.0;             // timed out -> no echo
  return (duration * 0.0343) / 2.0;           // speed of sound -> cm
}

bool readIR(uint8_t pin) {
  // Most IR obstacle modules pull LOW when an object/train is detected
  return digitalRead(pin) == LOW;
}

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  SerialBT.begin("SmartRailwayGate");

  pinMode(trigPin1, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);

  pinMode(buzzerPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);

  gateServoL.attach(servoPin1);
  gateServoR.attach(servoPin2);
  gateServoL.write(GATE_OPEN_ANGLE);
  gateServoR.write(GATE_OPEN_ANGLE);

  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, HIGH);
  digitalWrite(buzzerPin, LOW);

  Serial.println("Smart Railway Gate System - Initialized");
}

// ---------------------------------------------------------------------------
// Main Loop
// ---------------------------------------------------------------------------
void loop() {
  if (millis() - lastLoop < LOOP_DELAY_MS) return;
  lastLoop = millis();

  // 1) Read all four sensors -------------------------------------------------
  distanceL = readUltrasonicCM(trigPin1, echoPin1);
  distanceR = readUltrasonicCM(trigPin2, echoPin2);

  detected[0] = (distanceL > 0 && distanceL <= DISTANCE_THRESHOLD_CM); // US1 - left
  detected[1] = readIR(irPin1);                                       // IR1 - left
  detected[2] = (distanceR > 0 && distanceR <= DISTANCE_THRESHOLD_CM); // US2 - right
  detected[3] = readIR(irPin2);                                       // IR2 - right

  int detectionCount = detected[0] + detected[1] + detected[2] + detected[3];

  // 2) Fault-free reset --------------------------------------------------
  if (detectionCount == 0) {
    for (int i = 0; i < 4; i++) {
      faultLatched[i] = false;
      faultTimerRunning[i] = false;
    }
  } else {
    // 3) 3-out-of-4 majority voting with 1000 ms confirmation window ----
    for (int i = 0; i < 4; i++) {
      if (detectionCount == 3 && !detected[i]) {
        if (!faultTimerRunning[i]) {
          faultTimerRunning[i] = true;
          faultTimerStart[i] = millis();
        } else if (millis() - faultTimerStart[i] >= FAULT_CONFIRM_MS) {
          faultLatched[i] = true;   // permanently flag this node
        }
      } else {
        faultTimerRunning[i] = false; // condition no longer holds, reset timer
      }
    }
  }

  bool anyFault = faultLatched[0] || faultLatched[1] || faultLatched[2] || faultLatched[3];
  bool trainPresent = (detectionCount > 0) || anyFault;

  // 4) Actuate gate, LEDs, buzzer -----------------------------------------
  if (trainPresent) {
    gateServoL.write(GATE_CLOSED_ANGLE);
    gateServoR.write(GATE_CLOSED_ANGLE);
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin, HIGH);
  } else {
    gateServoL.write(GATE_OPEN_ANGLE);
    gateServoR.write(GATE_OPEN_ANGLE);
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(buzzerPin, LOW);
  }

  // 5) Telemetry over Bluetooth Serial @ 5 Hz ------------------------------
  if (millis() - lastTelemetry >= TX_INTERVAL_MS) {
    lastTelemetry = millis();
    sendTelemetry(trainPresent);
  }
}

// ---------------------------------------------------------------------------
// Telemetry packet: CSV string consumed by the MIT App Inventor app
// Format:
// GATE_STATUS,US_L,IR_L_state,US_R,IR_R_state,faultUSL,faultIRL,faultUSR,faultIRR
// ---------------------------------------------------------------------------
void sendTelemetry(bool trainPresent) {
  String packet = "";
  packet += trainPresent ? "GATE CLOSED" : "GATE OPEN";
  packet += ",";
  packet += (distanceL > 0 ? String(distanceL, 1) : "-1");
  packet += ",";
  packet += detected[0] ? "Detected" : "Clear";
  packet += ",";
  packet += (distanceR > 0 ? String(distanceR, 1) : "-1");
  packet += ",";
  packet += detected[2] ? "Detected" : "Clear";
  packet += ",";
  packet += faultLatched[0] ? "1" : "0";
  packet += ",";
  packet += faultLatched[1] ? "1" : "0";
  packet += ",";
  packet += faultLatched[2] ? "1" : "0";
  packet += ",";
  packet += faultLatched[3] ? "1" : "0";

  SerialBT.println(packet);
  Serial.println(packet);
}
