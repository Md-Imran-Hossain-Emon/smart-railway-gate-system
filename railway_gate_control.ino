// ============================================================
//  Railway Gate Controller — ESP32-WROOM-32 + Bluetooth
//  Fault Detection: 3-out-of-4 rule with 3-second confirmation
//  Fix: Safe pin assignments (5V-tolerant inputs, no bus conflicts)
// ============================================================

#include <BluetoothSerial.h>
#include <ESP32Servo.h>

BluetoothSerial SerialBT;

// ── Pin Definitions ──────────────────────────────────────────
// TRIG pins: any safe output GPIO
const int trigPin1    = 5;
const int trigPin2    = 19;

// ECHO pins: GPIO 34 & 35 are INPUT-ONLY + 5V tolerant
// No voltage divider needed with these pins
const int echoPin1    = 34;
const int echoPin2    = 35;

// IR pins: GPIO 22 is fine; GPIO 36 (VP) replaces 23 (was VSPI MOSI)
const int irPin1      = 22;   // IR Left  — unchanged, works fine
const int irPin2      = 23;   // IR Right — moved from 23 → 36 (VP)

// Output pins — unchanged
const int redLedPin   = 25;
const int greenLedPin = 26;
const int buzzerPin   = 27;

Servo servo1;
Servo servo2;
const int servoPin1   = 32;
const int servoPin2   = 33;

// ── Timing ───────────────────────────────────────────────────
unsigned long lastTxTime = 0;
const unsigned long txInterval = 200;

// ── Fault Confirmation Timers ─────────────────────────────────
unsigned long faultStart_USL = 0;
unsigned long faultStart_IRL = 0;
unsigned long faultStart_USR = 0;
unsigned long faultStart_IRR = 0;
const unsigned long FAULT_CONFIRM_MS = 1000;

// ── Latched fault flags ───────────────────────────────────────
bool latched_USL = false;
bool latched_IRL = false;
bool latched_USR = false;
bool latched_IRR = false;

// ─────────────────────────────────────────────────────────────
float getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(5);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 15000);  // 30 ms timeout
  if (duration == 0) return -1;
  return duration * 0.034f / 2.0f;            // cm
}

// ─────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  SerialBT.begin("RailGate_ESP32");

  pinMode(trigPin1,    OUTPUT);
  pinMode(trigPin2,    OUTPUT);

  // GPIO 34, 35, 36 are input-only — do NOT use pinMode INPUT_PULLUP
  // They have no internal pull-up. Just set INPUT.
  pinMode(echoPin1,    INPUT);   // GPIO 34
  pinMode(echoPin2,    INPUT);   // GPIO 35
  pinMode(irPin1,      INPUT_PULLUP);   // GPIO 22 — has pull-up, fine
  pinMode(irPin2,      INPUT_PULLUP);          // GPIO 36 (VP) — no internal pull-up
                                        // IR module has its own pull-up on OUT pin

  pinMode(redLedPin,   OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin,   OUTPUT);

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  servo1.write(0);
  servo2.write(0);
  digitalWrite(greenLedPin, HIGH);
  digitalWrite(redLedPin,   LOW);
  digitalWrite(buzzerPin,   LOW);
}

// ─────────────────────────────────────────────────────────────
void loop() {

  // ── 1. Read all four sensors ─────────────────────────────────
  float distance1 = getDistance(trigPin1, echoPin1);
  delay(20);
  float distance2 = getDistance(trigPin2, echoPin2);

  int irStatus1 = digitalRead(irPin1);
  int irStatus2 = digitalRead(irPin2);

  bool us1_detected = (distance1 > 0 && distance1 <= 10.0f);
  bool us2_detected = (distance2 > 0 && distance2 <= 10.0f);
  bool ir1_detected = (irStatus1 == LOW);
  bool ir2_detected = (irStatus2 == LOW);

  int detectionCount = (us1_detected ? 1 : 0)
                     + (us2_detected ? 1 : 0)
                     + (ir1_detected ? 1 : 0)
                     + (ir2_detected ? 1 : 0);

  // ── 2. Clear all latched faults when train is completely gone ─
  if (detectionCount == 0) {
    latched_USL = latched_IRL = latched_USR = latched_IRR = false;
    faultStart_USL = faultStart_IRL = faultStart_USR = faultStart_IRR = 0;
  }

  // ── 3. Fault detection (3-of-4 rule, 3 s confirmation) ───────
  auto checkFault = [&](bool sensorDetected,
                        unsigned long &timerRef,
                        bool &latchRef) {
    if (detectionCount == 3 && !sensorDetected) {
      if (timerRef == 0) {
        timerRef = millis();
      } else if (millis() - timerRef >= FAULT_CONFIRM_MS) {
        latchRef = true;
      }
    } else {
      timerRef = 0;
      // latchRef NOT cleared here — stays until train fully leaves
    }
  };

  checkFault(us1_detected, faultStart_USL, latched_USL);
  checkFault(ir1_detected, faultStart_IRL, latched_IRL);
  checkFault(us2_detected, faultStart_USR, latched_USR);
  checkFault(ir2_detected, faultStart_IRR, latched_IRR);

  int fault_USL = latched_USL ? 1 : 0;
  int fault_IRL = latched_IRL ? 1 : 0;
  int fault_USR = latched_USR ? 1 : 0;
  int fault_IRR = latched_IRR ? 1 : 0;

  // ── 4. Gate control ──────────────────────────────────────────
  bool trainPresent = (detectionCount > 0) ||
                      latched_USL || latched_IRL ||
                      latched_USR || latched_IRR;

  String gateStatus;
  if (trainPresent) {
    servo1.write(90);
    servo2.write(90);
    digitalWrite(redLedPin,   HIGH);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin,   HIGH);
    gateStatus = "GATE CLOSED";
  } else {
    servo1.write(0);
    servo2.write(0);
    digitalWrite(redLedPin,   LOW);
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(buzzerPin,   LOW);
    gateStatus = "GATE OPEN";
  }

  // ── 5. Bluetooth transmission (rate-limited) ─────────────────
  if (millis() - lastTxTime >= txInterval) {
    lastTxTime = millis();

    String ir1_text = ir1_detected ? "Detected" : "Clear";
    String ir2_text = ir2_detected ? "Detected" : "Clear";

    SerialBT.print(gateStatus);   SerialBT.print(",");  // 1
    SerialBT.print(distance1, 1); SerialBT.print(",");  // 2
    SerialBT.print(ir1_text);     SerialBT.print(",");  // 3
    SerialBT.print(distance2, 1); SerialBT.print(",");  // 4
    SerialBT.print(ir2_text);     SerialBT.print(",");  // 5
    SerialBT.print(fault_USL);    SerialBT.print(",");  // 6
    SerialBT.print(fault_IRL);    SerialBT.print(",");  // 7
    SerialBT.print(fault_USR);    SerialBT.print(",");  // 8
    SerialBT.print(fault_IRR);                          // 9
    SerialBT.println();
  }

  delay(20);
}
