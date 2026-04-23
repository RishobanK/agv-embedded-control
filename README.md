# AGV Embedded Control

Embedded C++ firmware for an autonomous guided vehicle (AGV) implementing sensor-based navigation and control logic.

## Features
- Ultrasonic sensor-based obstacle detection
- Motor control using relay interface
- State-based obstacle handling logic
- Non-blocking timing using millis()
- Audible feedback using buzzer

## Hardware
- ATmega2560
- Ultrasonic sensors (HC-SR04)
- Relay module
- Buzzer

## Software
- PlatformIO (Arduino framework)
- C++

## Project Structure
- `src/main.cpp` – Main firmware
- `platformio.ini` – Project configuration
