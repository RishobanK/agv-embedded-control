# AGV Embedded Control

Embedded firmware for an autonomous guided vehicle system with separate controllers for motion control and obstacle detection.

## System Architecture
- **Main controller**: handles AGV movement and line-following logic
- **Obstacle detection controller**: Arduino Uno-based ultrasonic sensing and stop/resume control

## Components
- Main AGV controller firmware
- Ultrasonic obstacle detection firmware
- Relay-based stop control
- Buzzer feedback

## Repository Structure
- `main-controller/` - primary AGV firmware
- `obstacle-detection-uno/` - ultrasonic sensor and safety logic firmware
