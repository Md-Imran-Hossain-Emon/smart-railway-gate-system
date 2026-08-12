
Conversation with Gemini
based on it give an attractive, adn professional readme . md code for github profile

              +-----------------------------------+
              |   Track Approach Sensing Array    |
              |  [US1 | IR1]        [US2 | IR2]   |
              +-----------------+-----------------+
                                |
                                v
              +-----------------------------------+
              |    ESP32 Edge Microcontroller     |
              |  - 3oo4 Voting Logic Engine       |
              |  - 1000ms Fault-Latching Timer    |
              |  - 200ms Telemetry Generator      |
              +-----------------+-----------------+
                                |
    +---------------------------+---------------------------+
    |                                                       |
    v                                                       v
+---------------+---------------+                       +---------------+---------------+
|  Actuation & Safety Systems   |                       |   Real-Time Telemetry Link    |
| - Dual Servo Gates (0° / 90°) |                       | - Bluetooth Serial (5 Hz)     |
| - Red/Green LED Status        |                       | - MIT App Inventor Mobile GUI |
| - High-Decibel Piezo Alarm    |                       | - Hardware Fault Diagnostics  |
+-------------------------------+                       +-------------------------------+


---

## 📊 System Operations & Flowchart

             +-----------------------------------+
             | START: Initialize ESP32, Servos 0° |
             |          Green LED ON             |
             +-----------------+-----------------+
                               |
                               v
             +-----------------------------------+
             | Read Sensors: US1, US2, IR1, IR2  |
             +-----------------+-----------------+
                               |
                               v
             +-----------------------------------+
             | Evaluate Detections & Compute     |
             |          detectionCount           |
             +-----------------+-----------------+
                               |
                 +-------------+-------------+
                 |                           |
        detectionCount == 0          detectionCount > 0
                 |                           |
                 v                           v
      +--------------------+       +--------------------+
      | Clear Latched      |       | Evaluate 3oo4      |
      | Faults & Timers    |       | Fault Condition    |
      +--------------------+       +---------+----------+
                 |                           |
                 |             +-------------+-------------+
                 |             |                           |
                 |     Sensor Clear for >1000ms    Sensor Normal / Timer Reset
                 |             |                           |
                 |             v                           v
                 |    +------------------+         +-------------------+
                 |    | Latch Fault = 1  |         | Determine Train   |
                 |    +--------+---------+         | Presence          |
                 |             |                   +---------+---------+
                 |             +-----------------------------+
                 |                                           |
                 v                                           v
      +--------------------+                       +-------------------+
      | Gate OPEN (0°)     |                       | Gate CLOSED (90°) |
      | Green LED ON       |                       | Red LED & Buzzer  |
      +---------+----------+                       +---------+---------+
                |                                            |
                +----------------------+---------------------+
                                       |
                                       v
                         < 200ms Telemetry Interval >
                                       |
                              +--------+--------+
                              | Stream CSV via  |
                              | Bluetooth BT    |
                              +-----------------+

---

## 📈 Experimental Performance & Telemetry Results

| Operating State Scenario | Sensor Pattern (`US1, IR1, US2, IR2`) | Detection Count | Fault Vector (`USL, IRL, USR, IRR`) | Physical Gate Position | Bluetooth Telemetry Packet Output (Sample) | Response Latency |
| :--- | :---: | :---: | :---: | :---: | :--- | :---: |
| **1. Empty Track (Idle)** | `Clear, Clear, Clear, Clear` | 0 | `0, 0, 0, 0` | 0° (Open), Green LED ON | `GATE OPEN, -1, Clear, -1, Clear, 0, 0, 0, 0` | `<5 ms` |
| **2. Train Entry (Nominal)** | `Detect, Detect, Detect, Detect` | 4 | `0, 0, 0, 0` | 90° (Closed), Red LED + Buzzer | `GATE CLOSED, 4.2, Detected, 5.1, Detected, 0, 0, 0, 0` | `<20 ms` |
| **3. Active Transit (Fault Window)** | `Clear, Detect, Detect, Detect` | 3 | `0, 0, 0, 0` | 90° (Closed), Red LED + Buzzer | `GATE CLOSED, -1, Detected, 4.8, Detected, 0, 0, 0, 0` | `1 ms (Timer Starts)` |
| **4. Active Transit (Fault Latched)** | `Clear, Detect, Detect, Detect` | 3 | `1, 0, 0, 0` | 90° (Closed), Red LED + Buzzer | `GATE CLOSED, -1, Detected, 4.5, Detected, 1, 0, 0, 0` | `≥1000 ms (Fault Locked)` |
| **5. Train Exit (Reset)** | `Clear, Clear, Clear, Clear` | 0 | `0, 0, 0, 0` | 0° (Open), Green LED ON | `GATE OPEN, -1, Clear, -1, Clear, 0, 0, 0, 0` | `<20 ms (Auto Clear)` |

---

## ⚡ Technical Specifications & Pin Configuration

| Parameter Variable | Fixed Value | Unit | Technical Purpose |
| :--- | :---: | :---: | :--- |
| `trigPin1, trigPin2` | `5, 19` | GPIO Index | Generates 10 µs sonic trigger pulses |
| `echoPin1, echoPin2` | `34, 35` | GPIO Index | Input pins tracking echo duration (Input-Only, Safe) |
| `irPin1, irPin2` | `22, 36` | GPIO Index | Tracks digital proximity logic from IR modules |
| `servoPin1, servoPin2` | `32, 33` | GPIO Index | Dedicated PWM channels driving barrier servos |
| `distanceThreshold` | `10.0` | cm | Maximum distance threshold for train detection |
| `FAULT_CONFIRM_MS` | `1000` | ms | Time window before missing sensor logic locks fault |
| `txInterval` | `200` | ms | Telemetry broadcast interval via Bluetooth (5 Hz) |
| `Pulse Timeout` | `30000` | µs | Upper bound execution limit for `pulseIn()` |

---

## 💰 Bill of Materials (BOM) & Cost Analysis

| Sl No. | Component Name | Quantity | Unit Price (BDT) | Total Cost (BDT) |
| :---: | :--- | :---: | :---: | :---: |
| 1 | ESP-32 WROOM-32 Microcontroller | 1 | 500 | 500 |
| 2 | HC-SR04 Ultrasonic Sensor | 2 | 150 | 300 |
| 3 | Infrared (IR) Proximity Sensor Module | 2 | 120 | 240 |
| 4 | SG90 Micro Servo Motor | 2 | 150 | 300 |
| 5 | Power Adapter / Battery Pack | 2 | 150 | 300 |
| 6 | Piezo Buzzer Alarm | 1 | 50 | 50 |
| 7 | Indicator LEDs & Current Limiting Resistors | 2 | 20 | 40 |
| 8 | Breadboard & Jumper Wire Set | 1 set | 250 | 250 |
| 9 | Structural Frame Materials | 1 set | 300 | 300 |
| 10 | Arduino UNO (Prototyping / Testing) | 1 | 650 | 650 |
| 11 | L298N Dual H-Bridge Motor Driver | 1 | 300 | 300 |
| 12 | TT Gear DC Motor | 2 | 150 | 300 |
| 13 | Robot Wheels | 4 | 25 | 100 |
| 14 | LM2596 DC-DC Buck Converter | 1 | 150 | 150 |
| **Total** | | | | **3,780 BDT** |

---

## 🚀 Future Enhancements

* 🌐 **Long-Range IoT Integration:** Incorporate Cellular LTE-M, NB-IoT, or LoRaWAN protocols for long-range central railway management monitoring.
* ☀️ **Solar Power Off-Grid System:** Deploy smart energy harvesting with battery management units (BMU) for continuous rural operation.
* 👁️ **Computer Vision Obstacle Detection:** Integrate edge AI cameras to identify stranded vehicles or pedestrians trapped inside closed gates.
* 🔮 **Predictive Maintenance:** Implement machine learning telemetry analysis to predict sensor drift and failure before catastrophic degradation.

---

## 📚 References

1. Y. Jiang, H. Low, and T. Clarke, *"Design and Formal Verification of a 3-out-of-4 (3oo4) Majority Voting Architecture for Safety-Critical Railway Interlocking Control Systems,"* IEEE Transactions on Reliability, vol. 71, no. 2, pp. 642–655, Jun. 2022.
2. M. Soualhi, M. El-Koujok, and N. Ramani, *"Heterogeneous Sensor Fusion Using Ultrasonic and Optical Arrays for Fault-Tolerant Object Detection in Harsh Industrial Environments,"* IEEE Sensors Journal, vol. 21, no. 11, pp. 12904–12915, Jun. 2021.
3. M. Al-Rousan and O. Al-Araidah, *"Edge-Computed Diagnostics and Real-Time Serial Telemetry Pipelines for Autonomous Grade Crossing Barriers Using 32-Bit Microcontrollers,"* Int. J. Civil & Struct. Eng. Automation, vol. 14, no. 3, pp. 201–214, Oct. 2023.
4. S. P. Edwards and V. R. Sastry, *"Mitigating Single-Point Failure Vulnerabilities in Level Crossing Infrastructure Through Hardware Redundancy and Temporal Latching Windows,"* IEEE Access, vol. 10, pp. 88510–88523, Aug. 2022.

---

<p center>
  Developed as part of <b>Course EEE 3100: Electronics Shop Practice</b><br>
  <b>Department of Electrical & Electronic Engineering (EEE)</b><br>
  <b>Rajshahi University of Engineering & Technology (RUET)</b>
</p>
"""

with open("README.md", "w", encoding="utf-8") as f:
    f.write(readme_content)

print("README.md generated successfully.")

Your README file is ready:

MD icon
README
 MD 
Open
📦 Markdown Source (README.md)
Markdown
# 🚆 Intelligent Autonomous Railway Crossing Safety Framework

> **An Edge-Computed, Fault-Tolerant Level Crossing Automation System featuring 3oo4 Multi-Sensor Redundancy and Real-Time Bluetooth Telemetry**

---

## 📌 Project Overview

Conventional railway level crossings represent a critical vulnerability in modern transportation infrastructure, often suffering from high accident rates due to delayed gate operations, single-point sensor failures, and a lack of remote telemetry[cite: 1]. 

This project presents an **Intelligent Autonomous Railway Crossing Safety Framework** designed to eliminate single-point-of-failure (SPOF) risks[cite: 1]. Powered by an **ESP32 microcontroller**, the system deploys a redundant, heterogeneous sensor array (Ultrasonic + Infrared) across track approaches[cite: 1]. Driven by a **3-out-of-4 (3oo4) majority voting logic** and a **1000 ms temporal fault-verification window**, the framework ensures uninterrupted safety barrier actuation even under single-sensor degradation, while streaming live diagnostics at **5 Hz** to a supervisory Android application[cite: 1].

---

## ✨ Key Features & Technical Highlights

* **🛡️ Multi-Sensor Redundancy Layer:** Uses a heterogeneous sensor matrix (2x HC-SR04 Ultrasonic + 2x Infrared) across two-way track zones (Left/Right) to prevent environmental blind spots (e.g., fog, rain, optical blockage)[cite: 1].
* **⚡ 3-out-of-4 (3oo4) Majority Voting Logic:** Software-driven voting algorithm that ensures safe defensive gate closures (90° servo deployment) in `<20 ms` upon train detection[cite: 1].
* **🔒 1000 ms Temporal Fault Latching:** Automatically flags, isolates, and latches failed or degraded sensor nodes (`fault = 1`) after a 1000 ms anomaly window without interrupting barrier protection[cite: 1].
* **📡 Real-Time Wireless Telemetry (5 Hz):** Packages distance metrics, detection state vectors, gate positions, and binary fault arrays into clean CSV streams transmitted over Bluetooth Serial every **200 ms**[cite: 1].
* **📱 Supervisory Mobile App Interface:** Developed with **MIT App Inventor** to render real-time graphical status indicators, system warnings, and hardware diagnostic logs for field operators[cite: 1].
* **⚡ Safe Hardware Pin Mapping:** Leverages ESP32 input-only, 5V-tolerant GPIOs (GPIO 34, 35, 36) to prevent pull-up conflicts and guarantee system stability[cite: 1].

---

## 🛠️ System Architecture & Working Principle

The framework is structured into three distinct operational layers[cite: 1]:
1. **Input Sensing Layer:** Distributed Ultrasonic (US1, US2) and Infrared (IR1, IR2) sensors continuously scan the Left and Right track approaches[cite: 1].
2. **Edge Processing & Decision Layer:** The ESP32 processes raw distance and proximity returns, evaluates the `detectionCount`, runs fault-latching timers, and enforces gate logic[cite: 1].
3. **Actuation & Telemetry Layer:** High-speed PWM signals trigger dual servo gate barriers, sound warning buzzers, update visual LED status indicators, and stream diagnostic packets via Bluetooth[cite: 1].

              +-----------------------------------+
              |   Track Approach Sensing Array    |
              |  [US1 | IR1]        [US2 | IR2]   |
              +-----------------+-----------------+
                                |
                                v
              +-----------------------------------+
              |    ESP32 Edge Microcontroller     |
              |  - 3oo4 Voting Logic Engine       |
              |  - 1000ms Fault-Latching Timer    |
              |  - 200ms Telemetry Generator      |
              +-----------------+-----------------+
                                |
    +---------------------------+---------------------------+
    |                                                       |
    v                                                       v
+---------------+---------------+                       +---------------+---------------+
|  Actuation & Safety Systems   |                       |   Real-Time Telemetry Link    |
| - Dual Servo Gates (0° / 90°) |                       | - Bluetooth Serial (5 Hz)     |
| - Red/Green LED Status        |                       | - MIT App Inventor Mobile GUI |
| - High-Decibel Piezo Alarm    |                       | - Hardware Fault Diagnostics  |
+-------------------------------+                       +-------------------------------+


---

## 📊 System Operations & Flowchart

             +-----------------------------------+
             | START: Initialize ESP32, Servos 0° |
             |          Green LED ON             |
             +-----------------+-----------------+
                               |
                               v
             +-----------------------------------+
             | Read Sensors: US1, US2, IR1, IR2  |
             +-----------------+-----------------+
                               |
                               v
             +-----------------------------------+
             | Evaluate Detections & Compute     |
             |          detectionCount           |
             +-----------------+-----------------+
                               |
                 +-------------+-------------+
                 |                           |
        detectionCount == 0          detectionCount > 0
                 |                           |
                 v                           v
      +--------------------+       +--------------------+
      | Clear Latched      |       | Evaluate 3oo4      |
      | Faults & Timers    |       | Fault Condition    |
      +--------------------+       +---------+----------+
                 |                           |
                 |             +-------------+-------------+
                 |             |                           |
                 |     Sensor Clear for >1000ms    Sensor Normal / Timer Reset
                 |             |                           |
                 |             v                           v
                 |    +------------------+         +-------------------+
                 |    | Latch Fault = 1  |         | Determine Train   |
                 |    +--------+---------+         | Presence          |
                 |             |                   +---------+---------+
                 |             +-----------------------------+
                 |                                           |
                 v                                           v
      +--------------------+                       +-------------------+
      | Gate OPEN (0°)     |                       | Gate CLOSED (90°) |
      | Green LED ON       |                       | Red LED & Buzzer  |
      +---------+----------+                       +---------+---------+
                |                                            |
                +----------------------+---------------------+
                                       |
                                       v
                         < 200ms Telemetry Interval >
                                       |
                              +--------+--------+
                              | Stream CSV via  |
                              | Bluetooth BT    |
                              +-----------------+

---

## 📈 Experimental Performance & Telemetry Results

| Operating State Scenario | Sensor Pattern (`US1, IR1, US2, IR2`) | Detection Count | Fault Vector (`USL, IRL, USR, IRR`) | Physical Gate Position | Bluetooth Telemetry Packet Output (Sample) | Response Latency |
| :--- | :---: | :---: | :---: | :---: | :--- | :---: |
| **1. Empty Track (Idle)** | `Clear, Clear, Clear, Clear` | 0 | `0, 0, 0, 0` | 0° (Open), Green LED ON | `GATE OPEN, -1, Clear, -1, Clear, 0, 0, 0, 0` | `<5 ms` |
| **2. Train Entry (Nominal)** | `Detect, Detect, Detect, Detect` | 4 | `0, 0, 0, 0` | 90° (Closed), Red LED + Buzzer | `GATE CLOSED, 4.2, Detected, 5.1, Detected, 0, 0, 0, 0` | `<20 ms` |
| **3. Active Transit (Fault Window)** | `Clear, Detect, Detect, Detect` | 3 | `0, 0, 0, 0` | 90° (Closed), Red LED + Buzzer | `GATE CLOSED, -1, Detected, 4.8, Detected, 0, 0, 0, 0` | `1 ms (Timer Starts)` |
| **4. Active Transit (Fault Latched)** | `Clear, Detect, Detect, Detect` | 3 | `1, 0, 0, 0` | 90° (Closed), Red LED + Buzzer | `GATE CLOSED, -1, Detected, 4.5, Detected, 1, 0, 0, 0` | `≥1000 ms (Fault Locked)` |
| **5. Train Exit (Reset)** | `Clear, Clear, Clear, Clear` | 0 | `0, 0, 0, 0` | 0° (Open), Green LED ON | `GATE OPEN, -1, Clear, -1, Clear, 0, 0, 0, 0` | `<20 ms (Auto Clear)` |

---

## ⚡ Technical Specifications & Pin Configuration

| Parameter Variable | Fixed Value | Unit | Technical Purpose |
| :--- | :---: | :---: | :--- |
| `trigPin1, trigPin2` | `5, 19` | GPIO Index | Generates 10 µs sonic trigger pulses |
| `echoPin1, echoPin2` | `34, 35` | GPIO Index | Input pins tracking echo duration (Input-Only, Safe) |
| `irPin1, irPin2` | `22, 36` | GPIO Index | Tracks digital proximity logic from IR modules |
| `servoPin1, servoPin2` | `32, 33` | GPIO Index | Dedicated PWM channels driving barrier servos |
| `distanceThreshold` | `10.0` | cm | Maximum distance threshold for train detection |
| `FAULT_CONFIRM_MS` | `1000` | ms | Time window before missing sensor logic locks fault |
| `txInterval` | `200` | ms | Telemetry broadcast interval via Bluetooth (5 Hz) |
| `Pulse Timeout` | `30000` | µs | Upper bound execution limit for `pulseIn()` |

---

## 💰 Bill of Materials (BOM) & Cost Analysis

| Sl No. | Component Name | Quantity | Unit Price (BDT) | Total Cost (BDT) |
| :---: | :--- | :---: | :---: | :---: |
| 1 | ESP-32 WROOM-32 Microcontroller | 1 | 500 | 500 |
| 2 | HC-SR04 Ultrasonic Sensor | 2 | 150 | 300 |
| 3 | Infrared (IR) Proximity Sensor Module | 2 | 120 | 240 |
| 4 | SG90 Micro Servo Motor | 2 | 150 | 300 |
| 5 | Power Adapter / Battery Pack | 2 | 150 | 300 |
| 6 | Piezo Buzzer Alarm | 1 | 50 | 50 |
| 7 | Indicator LEDs & Current Limiting Resistors | 2 | 20 | 40 |
| 8 | Breadboard & Jumper Wire Set | 1 set | 250 | 250 |
| 9 | Structural Frame Materials | 1 set | 300 | 300 |
| 10 | Arduino UNO (Prototyping / Testing) | 1 | 650 | 650 |
| 11 | L298N Dual H-Bridge Motor Driver | 1 | 300 | 300 |
| 12 | TT Gear DC Motor | 2 | 150 | 300 |
| 13 | Robot Wheels | 4 | 25 | 100 |
| 14 | LM2596 DC-DC Buck Converter | 1 | 150 | 150 |
| **Total** | | | | **3,780 BDT** |

---

## 🚀 Future Enhancements

* 🌐 **Long-Range IoT Integration:** Incorporate Cellular LTE-M, NB-IoT, or LoRaWAN protocols for long-range central railway management monitoring[cite: 1].
* ☀️ **Solar Power Off-Grid System:** Deploy smart energy harvesting with battery management units (BMU) for continuous rural operation[cite: 1].
* 👁️ **Computer Vision Obstacle Detection:** Integrate edge AI cameras to identify stranded vehicles or pedestrians trapped inside closed gates[cite: 1].
* 🔮 **Predictive Maintenance:** Implement machine learning telemetry analysis to predict sensor drift and failure before catastrophic degradation[cite: 1].

---

## 📚 References

1. Y. Jiang, H. Low, and T. Clarke, *"Design and Formal Verification of a 3-out-of-4 (3oo4) Majority Voting Architecture for Safety-Critical Railway Interlocking Control Systems,"* IEEE Transactions on Reliability, vol. 71, no. 2, pp. 642–655, Jun. 2022[cite: 1].
2. M. Soualhi, M. El-Koujok, and N. Ramani, *"Heterogeneous Sensor Fusion Using Ultrasonic and Optical Arrays for Fault-Tolerant Object Detection in Harsh Industrial Environments,"* IEEE Sensors Journal, vol. 21, no. 11, pp. 12904–12915, Jun. 2021[cite: 1].
3. M. Al-Rousan and O. Al-Araidah, *"Edge-Computed Diagnostics and Real-Time Serial Telemetry Pipelines for Autonomous Grade Crossing Barriers Using 32-Bit Microcontrollers,"* Int. J. Civil & Struct. Eng. Automation, vol. 14, no. 3, pp. 201–214, Oct. 2023[cite: 1].
4. S. P. Edwards and V. R. Sastry, *"Mitigating Single-Point Failure Vulnerabilities in Level Crossing Infrastructure Through Hardware Redundancy and Temporal Latching Windows,"* IEEE Access, vol. 10, pp. 88510–88523, Aug. 2022[cite: 1].

---

<p align="center">
  Developed as part of <b>Course EEE 3100: Electronics Shop Practice</b><br>
  <b>Department of Electrical & Electronic Engineering (EEE)</b><br>
  <b>Rajshahi University of Engineering & Technology (RUET)</b>
</p>
Gemini is AI and can make mistakes.




# 🚆 Intelligent Autonomous Railway Crossing Safety Framework

> **An Edge-Computed, Fault-Tolerant Level Crossing Automation System featuring 3oo4 Multi-Sensor Redundancy and Real-Time Bluetooth Telemetry**

---

## 📌 Project Overview

Conventional railway level crossings represent a critical vulnerability in modern transportation infrastructure, often suffering from high accident rates due to delayed gate operations, single-point sensor failures, and a lack of remote telemetry. 

This project presents an **Intelligent Autonomous Railway Crossing Safety Framework** designed to eliminate single-point-of-failure (SPOF) risks. Powered by an **ESP32 microcontroller**, the system deploys a redundant, heterogeneous sensor array (Ultrasonic + Infrared) across track approaches. Driven by a **3-out-of-4 (3oo4) majority voting logic** and a **1000 ms temporal fault-verification window**, the framework ensures uninterrupted safety barrier actuation even under single-sensor degradation, while streaming live diagnostics at **5 Hz** to a supervisory Android application.

---

## ✨ Key Features & Technical Highlights

* **🛡️ Multi-Sensor Redundancy Layer:** Uses a heterogeneous sensor matrix (2x HC-SR04 Ultrasonic + 2x Infrared) across two-way track zones (Left/Right) to prevent environmental blind spots (e.g., fog, rain, optical blockage).
* **⚡ 3-out-of-4 (3oo4) Majority Voting Logic:** Software-driven voting algorithm that ensures safe defensive gate closures (90° servo deployment) in `<20 ms` upon train detection.
* **🔒 1000 ms Temporal Fault Latching:** Automatically flags, isolates, and latches failed or degraded sensor nodes (`fault = 1`) after a 1000 ms anomaly window without interrupting barrier protection.
* **📡 Real-Time Wireless Telemetry (5 Hz):** Packages distance metrics, detection state vectors, gate positions, and binary fault arrays into clean CSV streams transmitted over Bluetooth Serial every **200 ms**.
* **📱 Supervisory Mobile App Interface:** Developed with **MIT App Inventor** to render real-time graphical status indicators, system warnings, and hardware diagnostic logs for field operators.
* **⚡ Safe Hardware Pin Mapping:** Leverages ESP32 input-only, 5V-tolerant GPIOs (GPIO 34, 35, 36) to prevent pull-up conflicts and guarantee system stability.

---

## 🛠️ System Architecture & Working Principle

The framework is structured into three distinct operational layers:
1. **Input Sensing Layer:** Distributed Ultrasonic (US1, US2) and Infrared (IR1, IR2) sensors continuously scan the Left and Right track approaches.
2. **Edge Processing & Decision Layer:** The ESP32 processes raw distance and proximity returns, evaluates the `detectionCount`, runs fault-latching timers, and enforces gate logic.
3. **Actuation & Telemetry Layer:** High-speed PWM signals trigger dual servo gate barriers, sound warning buzzers, update visual LED status indicators, and stream diagnostic packets via Bluetooth.

```
                  +-----------------------------------+
                  |   Track Approach Sensing Array    |
                  |  [US1 | IR1]        [US2 | IR2]   |
                  +-----------------+-----------------+
                                    |
                                    v
                  +-----------------------------------+
                  |    ESP32 Edge Microcontroller     |
                  |  - 3oo4 Voting Logic Engine       |
                  |  - 1000ms Fault-Latching Timer    |
                  |  - 200ms Telemetry Generator      |
                  +-----------------+-----------------+
                                    |
        +---------------------------+---------------------------+
        |                                                       |
        v                                                       v
+---------------+---------------+                       +---------------+---------------+
|  Actuation & Safety Systems   |                       |   Real-Time Telemetry Link    |
| - Dual Servo Gates (0° / 90°) |                       | - Bluetooth Serial (5 Hz)     |
| - Red/Green LED Status        |                       | - MIT App Inventor Mobile GUI |
| - High-Decibel Piezo Alarm    |                       | - Hardware Fault Diagnostics  |
+-------------------------------+                       +-------------------------------+
```

---

## 📊 System Operations & Flowchart

```
                 +-----------------------------------+
                 | START: Initialize ESP32, Servos 0° |
                 |          Green LED ON             |
                 +-----------------+-----------------+
                                   |
                                   v
                 +-----------------------------------+
                 | Read Sensors: US1, US2, IR1, IR2  |
                 +-----------------+-----------------+
                                   |
                                   v
                 +-----------------------------------+
                 | Evaluate Detections & Compute     |
                 |          detectionCount           |
                 +-----------------+-----------------+
                                   |
                     +-------------+-------------+
                     |                           |
            detectionCount == 0          detectionCount > 0
                     |                           |
                     v                           v
          +--------------------+       +--------------------+
          | Clear Latched      |       | Evaluate 3oo4      |
          | Faults & Timers    |       | Fault Condition    |
          +--------------------+       +---------+----------+
                     |                           |
                     |             +-------------+-------------+
                     |             |                           |
                     |     Sensor Clear for >1000ms    Sensor Normal / Timer Reset
                     |             |                           |
                     |             v                           v
                     |    +------------------+         +-------------------+
                     |    | Latch Fault = 1  |         | Determine Train   |
                     |    +--------+---------+         | Presence          |
                     |             |                   +---------+---------+
                     |             +-----------------------------+
                     |                                           |
                     v                                           v
          +--------------------+                       +-------------------+
          | Gate OPEN (0°)     |                       | Gate CLOSED (90°) |
          | Green LED ON       |                       | Red LED & Buzzer  |
          +---------+----------+                       +---------+---------+
                    |                                            |
                    +----------------------+---------------------+
                                           |
                                           v
                             < 200ms Telemetry Interval >
                                           |
                                  +--------+--------+
                                  | Stream CSV via  |
                                  | Bluetooth BT    |
                                  +-----------------+
```

---

## 📈 Experimental Performance & Telemetry Results

| Operating State Scenario | Sensor Pattern (`US1, IR1, US2, IR2`) | Detection Count | Fault Vector (`USL, IRL, USR, IRR`) | Physical Gate Position | Bluetooth Telemetry Packet Output (Sample) | Response Latency |
| :--- | :---: | :---: | :---: | :---: | :--- | :---: |
| **1. Empty Track (Idle)** | `Clear, Clear, Clear, Clear` | 0 | `0, 0, 0, 0` | 0° (Open), Green LED ON | `GATE OPEN, -1, Clear, -1, Clear, 0, 0, 0, 0` | `<5 ms` |
| **2. Train Entry (Nominal)** | `Detect, Detect, Detect, Detect` | 4 | `0, 0, 0, 0` | 90° (Closed), Red LED + Buzzer | `GATE CLOSED, 4.2, Detected, 5.1, Detected, 0, 0, 0, 0` | `<20 ms` |
| **3. Active Transit (Fault Window)** | `Clear, Detect, Detect, Detect` | 3 | `0, 0, 0, 0` | 90° (Closed), Red LED + Buzzer | `GATE CLOSED, -1, Detected, 4.8, Detected, 0, 0, 0, 0` | `1 ms (Timer Starts)` |
| **4. Active Transit (Fault Latched)** | `Clear, Detect, Detect, Detect` | 3 | `1, 0, 0, 0` | 90° (Closed), Red LED + Buzzer | `GATE CLOSED, -1, Detected, 4.5, Detected, 1, 0, 0, 0` | `≥1000 ms (Fault Locked)` |
| **5. Train Exit (Reset)** | `Clear, Clear, Clear, Clear` | 0 | `0, 0, 0, 0` | 0° (Open), Green LED ON | `GATE OPEN, -1, Clear, -1, Clear, 0, 0, 0, 0` | `<20 ms (Auto Clear)` |

---

## ⚡ Technical Specifications & Pin Configuration

| Parameter Variable | Fixed Value | Unit | Technical Purpose |
| :--- | :---: | :---: | :--- |
| `trigPin1, trigPin2` | `5, 19` | GPIO Index | Generates 10 µs sonic trigger pulses |
| `echoPin1, echoPin2` | `34, 35` | GPIO Index | Input pins tracking echo duration (Input-Only, Safe) |
| `irPin1, irPin2` | `22, 36` | GPIO Index | Tracks digital proximity logic from IR modules |
| `servoPin1, servoPin2` | `32, 33` | GPIO Index | Dedicated PWM channels driving barrier servos |
| `distanceThreshold` | `10.0` | cm | Maximum distance threshold for train detection |
| `FAULT_CONFIRM_MS` | `1000` | ms | Time window before missing sensor logic locks fault |
| `txInterval` | `200` | ms | Telemetry broadcast interval via Bluetooth (5 Hz) |
| `Pulse Timeout` | `30000` | µs | Upper bound execution limit for `pulseIn()` |

---

## 💰 Bill of Materials (BOM) & Cost Analysis

| Sl No. | Component Name | Quantity | Unit Price (BDT) | Total Cost (BDT) |
| :---: | :--- | :---: | :---: | :---: |
| 1 | ESP-32 WROOM-32 Microcontroller | 1 | 500 | 500 |
| 2 | HC-SR04 Ultrasonic Sensor | 2 | 150 | 300 |
| 3 | Infrared (IR) Proximity Sensor Module | 2 | 120 | 240 |
| 4 | SG90 Micro Servo Motor | 2 | 150 | 300 |
| 5 | Power Adapter / Battery Pack | 2 | 150 | 300 |
| 6 | Piezo Buzzer Alarm | 1 | 50 | 50 |
| 7 | Indicator LEDs & Current Limiting Resistors | 2 | 20 | 40 |
| 8 | Breadboard & Jumper Wire Set | 1 set | 250 | 250 |
| 9 | Structural Frame Materials | 1 set | 300 | 300 |
| 10 | Arduino UNO (Prototyping / Testing) | 1 | 650 | 650 |
| 11 | L298N Dual H-Bridge Motor Driver | 1 | 300 | 300 |
| 12 | TT Gear DC Motor | 2 | 150 | 300 |
| 13 | Robot Wheels | 4 | 25 | 100 |
| 14 | LM2596 DC-DC Buck Converter | 1 | 150 | 150 |
| **Total** | | | | **3,780 BDT** |

---

## 🚀 Future Enhancements

* 🌐 **Long-Range IoT Integration:** Incorporate Cellular LTE-M, NB-IoT, or LoRaWAN protocols for long-range central railway management monitoring.
* ☀️ **Solar Power Off-Grid System:** Deploy smart energy harvesting with battery management units (BMU) for continuous rural operation.
* 👁️ **Computer Vision Obstacle Detection:** Integrate edge AI cameras to identify stranded vehicles or pedestrians trapped inside closed gates.
* 🔮 **Predictive Maintenance:** Implement machine learning telemetry analysis to predict sensor drift and failure before catastrophic degradation.

---

## 📚 References

1. Y. Jiang, H. Low, and T. Clarke, *"Design and Formal Verification of a 3-out-of-4 (3oo4) Majority Voting Architecture for Safety-Critical Railway Interlocking Control Systems,"* IEEE Transactions on Reliability, vol. 71, no. 2, pp. 642–655, Jun. 2022.
2. M. Soualhi, M. El-Koujok, and N. Ramani, *"Heterogeneous Sensor Fusion Using Ultrasonic and Optical Arrays for Fault-Tolerant Object Detection in Harsh Industrial Environments,"* IEEE Sensors Journal, vol. 21, no. 11, pp. 12904–12915, Jun. 2021.
3. M. Al-Rousan and O. Al-Araidah, *"Edge-Computed Diagnostics and Real-Time Serial Telemetry Pipelines for Autonomous Grade Crossing Barriers Using 32-Bit Microcontrollers,"* Int. J. Civil & Struct. Eng. Automation, vol. 14, no. 3, pp. 201–214, Oct. 2023.
4. S. P. Edwards and V. R. Sastry, *"Mitigating Single-Point Failure Vulnerabilities in Level Crossing Infrastructure Through Hardware Redundancy and Temporal Latching Windows,"* IEEE Access, vol. 10, pp. 88510–88523, Aug. 2022.

---

<p center>
  Developed as part of <b>Course EEE 3100: Electronics Shop Practice</b><br>
  <b>Department of Electrical & Electronic Engineering (EEE)</b><br>
  <b>Rajshahi University of Engineering & Technology (RUET)</b>
</p>
README.md
Displaying README.md.
