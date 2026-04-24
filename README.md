# 🚗 AGV Embedded Control and Safety System

Dual-controller embedded firmware for an Autonomous Guided Vehicle (AGV), combining real-time line following, route marker detection, motor control, and independent ultrasonic obstacle safety logic.

---

## 🔎 Overview

This AGV system is built using a **dual-controller architecture**:

- **Main Controller**  
  Handles line following, motor control, route logic, turning, timed stops, and EEPROM-based calibration.

- **Safety Controller**  
  Independently monitors ultrasonic sensors and overrides motion using relay-based stop/resume control.

This separation ensures **better reliability, modularity, and safety** in real-time operation.

---

## ✨ Key Features

- 8-channel IR line-following
- PD-based steering correction
- Custom route marker detection
- Controlled turning and smooth stopping
- EEPROM-based sensor calibration loading
- Manual start/stop via push buttons
- 3-sensor ultrasonic obstacle monitoring
- Relay-based emergency stop control
- Buzzer feedback during motion and obstacle events
- Grace-period resume logic after obstacle clearance

---

## 🧠 System Architecture

```text
IR Sensor Array → Main Controller → Motor Driver → Motors
                         ↑
                 Push Buttons

Ultrasonic Sensors → Safety Controller → Relay Stop Control
                                       └→ Buzzer Alert

```

---

## 🧩 Firmware Structure

agv-embedded-control/
├── src/
│   ├── main_controller.cpp        # AGV navigation & motor control
│   └── obstacle_detection_uno.cpp # Ultrasonic safety controller
├── lib/
│   └── BeeLineSensorPro/          # Line sensor library
├── platformio.ini                 # Multi-target build configuration
└── README.md

---

## ⚙️ PlatformIO Configuration

This project uses **multiple build environments** for different controllers:

- `main_controller` → Arduino Mega / AGV control
- `obstacle_detection_uno` → Arduino Uno / safety controller

---

## 🛠 Build Instructions

Build main controller firmware:

`pio run -e main_controller`

Build obstacle detection firmware:

`pio run -e obstacle_detection_uno`

Upload main controller firmware:

`pio run -e main_controller -t upload`

Upload obstacle controller firmware:

`pio run -e obstacle_detection_uno -t upload`

---

## 🔌 Hardware Used

- Arduino Mega / equivalent main controller
- Arduino Uno safety controller
- 8-channel IR line sensor array
- 3 ultrasonic sensors
- Motor driver
- DC motors
- Relay module
- Buzzer
- Push buttons

---

## ⚡ Control Logic Highlights

### Main Controller

- PD-based line-following using 8 IR sensors
- Motor speed control using PWM
- Route marker detection using sensor pattern logic
- Smooth stopping and controlled turning sequences
- EEPROM-based calibration loading

### Safety Controller

- Monitors 3 ultrasonic sensors
- Detects obstacles below the threshold distance
- Activates relay output to stop the AGV
- Uses grace-period logic before resuming after obstacle clearance
- Provides buzzer alerts during obstacle events

---

## 🎯 Why Two Controllers?

The system separates **navigation logic** and **safety logic** into two independent controllers.

This improves:

- Fault isolation
- System reliability
- Debugging simplicity
- Real-time safety responsiveness

---

## 🚀 Future Improvements

- Convert blocking delays to non-blocking state machines
- Add structured communication protocol between controllers
- Improve route configurability
- Add battery monitoring and diagnostics
- Integrate logging and telemetry

---

## 👤 Author

**Rishoban Kandeepan**  
Embedded Systems | Sensor Integration | Real-Time Control

- GitHub: https://github.com/RishobanK
- Portfolio: https://www.notion.so/Hi-I-m-Rishoban-Kandeepan-251a48156db080fa80acd660c4368469
- LinkedIn: https://linkedin.com/in/rishoban-kandeepan
