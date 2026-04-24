# AGV Embedded Control and Safety System

Dual-controller embedded firmware for an Autonomous Guided Vehicle (AGV), combining real-time line following, route marker detection, motor control, and independent ultrasonic obstacle safety logic.

## Overview

This AGV system uses two microcontroller boards:

- **Main Controller** — handles line following, motor control, route logic, turning, timed stops, and calibration loading
- **Safety Controller** — monitors ultrasonic sensors and triggers relay-based stop/resume control when obstacles are detected

The system separates navigation and safety tasks to improve modularity, debugging, and reliability.

## Key Features

- 8-channel IR line-following
- PD-based steering correction
- Custom route marker detection
- Controlled turning and smooth stopping
- EEPROM-based sensor calibration loading
- Manual start/stop button control
- 3-sensor ultrasonic obstacle monitoring
- Relay-based emergency stop control
- Buzzer feedback during motion and obstacle events
- Grace-period resume logic after obstacle clearance

## System Architecture

```text
IR Sensor Array ──> Main Controller ──> Motor Driver ──> Motors
                         ▲
                         │
Push Buttons ────────────┘

Ultrasonic Sensors ──> Safety Controller ──> Relay Stop Control
                               │
                               └──> Buzzer Alert
