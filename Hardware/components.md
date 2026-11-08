# Components

This document lists the main hardware components, their specifications, quantities, and roles in the self-balancing robot.

## Main Components

| Component | Quantity | Specifications | Purpose |
|---|---:|---|---|
| ESP32 Development Board | 1 | ESP32-WROOM-32, 3.3V logic | Main microcontroller. Processes sensor data and controls the motors. |
| MPU6050 | 1 | 6-axis IMU, 3-axis accelerometer, 3-axis gyroscope, I2C interface | Measures the robot's acceleration and angular velocity. |
| TB6612FNG Motor Driver | 1 | Dual H-bridge, 2 DC motor channels | Controls the speed and direction of both motors. |
| TT DC Geared Motors | 2 |  | Drive the wheels and maintain balance. |
| Wheels | 2 |  | Provide traction and movement. |
| LiPo Battery Cells | 2 | 3.7V, 2200mAh each | Supply power to the robot. |
| DC-DC Buck Converter | 1 | 5V output | Regulates battery voltage to the required operating voltage. |
| Perfboard | 1 | 12 cm × 8 cm | Used to assemble and solder the electronic circuit. |
| Jumper Wires | As needed |  | Connect the electronic components. |
| Robot Frame | 1 | Custom-designed in SolidWorks | Supports and holds the electronic and mechanical components. |
| Screws and Fasteners | As needed |  | Secure the frame, motors, and other components. |

## Software and Development Tools

| Tool | Purpose |
|---|---|
| Arduino IDE | Writing and uploading firmware to the ESP32. |
| C/C++ | Programming the robot's control system. |
| SolidWorks  | Designing the robot frame and mechanical parts. |
| 3D Printing | Manufacturing the designed mechanical parts. |

## Component Roles

- **ESP32:** Reads sensor measurements, estimates the robot's orientation, and executes the PID control algorithm.
- **MPU6050:** Provides accelerometer and gyroscope measurements to estimate the robot's tilt angle.
- **TB6612FNG:** Receives control signals from the ESP32 and drives the two DC motors.
- **DC Motors:** Adjust their speed and direction to keep the robot upright.
- **LiPo Batteries:** Provide electrical power to the system.
- **Buck Converter:** Regulates the battery voltage to provide a stable supply for the electronics.

## Assembly and Design

- The electronic circuit is assembled and soldered onto a 12 cm × 8 cm perfboard.
- The robot frame was designed using SolidWorks.
- The mechanical parts are manufactured using 3D printing.
- The ESP32 communicates with the MPU6050 through the I2C interface.
- The PID control algorithm processes the sensor measurements and generates motor control signals.

## Notes

- Battery configuration (series or parallel) determines the total voltage and capacity.
- Component specifications may be updated as the project develops.
- The design combines custom mechanical parts with electronic components to achieve a self-balancing system.
