# Components

This document lists the main hardware components used to build the self-balancing robot.

## Main Components

| Component | Quantity | Purpose |
|---|---:|---|
| ESP32 Development Board | 1 | Main microcontroller. Processes sensor data and controls the motors. |
| MPU6050 | 1 | 6-axis accelerometer and gyroscope used to measure the robot's tilt and angular velocity. |
| TB6612FNG Motor Driver | 1 | Controls the speed and direction of both DC motors. |
| TT DC Geared Motors | 2 | Drive the robot's wheels and maintain balance. |
| Wheels | 2 | Provide traction and movement. |
| Battery | 1 | Supplies power to the robot. |
| DC-DC Buck Converter | 1 | Regulates the battery voltage to the required operating voltage. |
| Perfboard | 1 | Used to assemble and solder the electronic circuit. |
| Jumper Wires | As needed | Connect the electronic components. |
| Robot Frame | 1 | Supports and holds the electronic and mechanical components. |
| Screws and Fasteners | As needed | Secure the frame, motors, and other components. |

## Software and Development Tools

| Tool | Purpose |
|---|---|
| Arduino IDE | Writing and uploading the firmware to the ESP32. |
| C/C++ | Programming the robot's control system. |
| SolidWorks | Designing the robot frame and mechanical parts. |
| 3D Printing | Manufacturing the designed mechanical parts. |

## Component Roles

- **ESP32:** Reads sensor measurements, calculates the robot's orientation, and runs the PID control algorithm.
- **MPU6050:** Provides accelerometer and gyroscope measurements to estimate the robot's tilt angle.
- **TB6612FNG:** Receives control signals from the ESP32 and drives the two motors.
- **DC Motors:** Adjust their speed and direction to keep the robot upright.
- **Buck Converter:** Provides a regulated voltage for the electronics.

## Notes

- The electronic circuit is assembled and soldered on a perfboard.
- The robot frame was designed in SolidWorks and manufactured using 3D printing.
- Component quantities and specifications may be updated as the project evolves.
