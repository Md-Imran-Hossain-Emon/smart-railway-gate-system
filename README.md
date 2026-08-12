# Smart Railway Gate Automation & Fault Detection System

An IoT-based intelligent autonomous railway level crossing safety and monitoring system built using ESP32, dual servo motors, ultrasonic/IR sensors, and Bluetooth telemetry.

---

## Key Features

* **Multi-Sensor Redundancy:** Integrates Ultrasonic and IR sensors for reliable train arrival/departure detection.
* **Automated Barrier Control:** Dual servo-actuated gate positioning based on real-time sensor logic.
* **Fault Detection Algorithm:** Continuous system diagnostic loop to identify sensor failure or obstruction in real-time.
* **Wireless Telemetry:** Real-time data logging and manual override control via a custom MIT App Inventor Android Application.

---

## System Architecture & Hardware

* **Microcontroller:** ESP32 Board
* **Sensors:** Ultrasonic Sensor (HC-SR04), Infrared (IR) Sensors
* **Actuators:** SG90 Servo Motors, Buzzer/LED Indicators
* **Communication:** ESP32 Classical Bluetooth (Serial Telemetry)

---

## Repository Structure

```text
├── App/                # MIT App Inventor source code (.apk) & UI design screenshots
├── Code/               # Embedded C++ source code (.ino) & technical documentation
├── Documentation/      # Project report, system flowchart, and poster
└── Hardware & Media/   # Circuit schematics and prototype physical setup images
