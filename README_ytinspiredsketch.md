# YT-Inspired Self-Balancing Robot

Welcome to the `ytinspiredsketch_mar17a.ino` documentation! This sketch is a complete Arduino project for an ESP32 self-balancing robot. Whether you followed a tutorial or are looking for a complete framework, this codebase covers sensor reading, PID control, and motor driving over Wi-Fi!

## Quick Start Guide

### 1. Wiring it up
Connect your ESP32 to your MPU6050 and motor driver. Check the code for exact pinouts:
- **I2C Pins**: `21` (SDA) and `22` (SCL) for the MPU6050.
- **Motor Pins**: `25` (ENA), `26` (IN1), `27` (IN2) for the left motor. `33` (ENB), `14` (IN3), `12` (IN4) for the right motor.
- **Interrupt Config**: Run a wire from MPU6050 `INT` to ESP32 Pin `2`.

### 2. Required Libraries
Make sure you install the following libraries via the Arduino IDE Library Manager or ZIP file:
- `PID_v1`
- `LMotorController`
- `I2Cdev`
- `MPU6050_6Axis_MotionApps20` (Often found in jrowberg/i2cdevlib)

### 3. Web UI Calibration
Tuning a self-balancing robot requires patience. To spare you from constantly flashing new code, this project hosts a Wi-Fi portal!
1. Connect your phone or laptop to the Wi-Fi network **ESP32_BALANCE** (password: **12345678**).
2. Go to your browser and enter the ESP32's gateway IP (usually `http://192.168.4.1`).
3. You will see a tuning portal. Enter new values for Kp, Ki, or Kd and press submit to see the real-time effect on your robot's behavior!

## Trouble-shooting
- **"FIFO Overflow!" in Serial Monitor**: Usually happens if the loop runs too slowly or the interrupt isn't firing fast enough. 
- **DMP Init failed**: Double-check the 3.3V power to the MPU6050 and that SDA/SCL lines are properly connected.
- **Motors running backwards**: Swap the IN1/IN2 or IN3/IN4 wires, or modify `motorSpeedFactorLeft`/`motorSpeedFactorRight` in code.
