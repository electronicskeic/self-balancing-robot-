# Self-Balancing Robot - Original Implementation

This directory contains `selfcodesketch_mar17a.ino`, the primary codebase for an ESP32-based self-balancing robot. This firmware relies on an MPU6050 accelerometer/gyroscope, a PID control loop, and an integrated Web UI for live tuning via Wi-Fi.

## Hardware Requirements
* **Microcontroller**: ESP32
* **IMU Sensor**: MPU6050
* **Motor Driver**: L298N (or similar driver via `LMotorController`)
* **Engines**: 2x DC Motors with Encoders (wheels)

## Pin Mapping
### MPU6050 Connections (I2C)
* **SDA**: GPIO 21
* **SCL**: GPIO 22
* **INT**: GPIO 2 (Hardware Interrupt)

### Motor Driver Connections
* **Motor A (Left)**: ENA = GPIO 25, IN1 = GPIO 26, IN2 = GPIO 27
* **Motor B (Right)**: ENB = GPIO 33, IN3 = GPIO 14, IN4 = GPIO 12

## Features
1. **DMP Processing**: Utilizes the MPU6050 onboard Digital Motion Processor (DMP) to calculate Quaternion and Gravity vectors, outputting Yaw, Pitch, Roll.
2. **PID Loop**: Actively computes motor speed and direction based on the current pitch angle versus the setpoint (180 degrees).
3. **Web Server Tuning**: Hosts an Access Point (SSID: `ESP32_BALANCE`, Pass: `12345678`). Access `http://192.168.4.1` (typically) to tune Kp, Ki, and Kd on the fly without recompiling.

## Initial Tuning Parameters
The default PID values act as a baseline:
* **Kp**: 60
* **Ki**: 270
* **Kd**: 2.2

*Note: You may need to modify the initial gyro and accel offsets located in `setup()` based on your MPU6050's specific calibration values.*
