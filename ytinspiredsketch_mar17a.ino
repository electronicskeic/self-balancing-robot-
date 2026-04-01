#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <math.h>
#include <PID_v1.h>
#include <LMotorController.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// MPU
MPU6050 mpu;

// MPU control/status
bool dmpReady = false;
uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];

// orientation
Quaternion q;
VectorFloat gravity;
float ypr[3];

// interrupt
volatile bool mpuInterrupt = false;
#define INTERRUPT_PIN 2

void dmpDataReady() {
  mpuInterrupt = true;
}

// PID
double input, output;
double setpoint = 180;

// tuning (start here)
double Kp = 60;
double Ki = 270;
double Kd = 2.2;

PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

// Motor pins
int IN1 = 26, IN2 = 27;
int IN3 = 14, IN4 = 12;
int ENA = 25, ENB = 33;

// motor tuning
float motorSpeedFactorLeft = 1.0;
float motorSpeedFactorRight = 1.0;
#define MIN_ABS_SPEED 30

LMotorController motorController(ENA, IN1, IN2, ENB, IN3, IN4,
                                 motorSpeedFactorLeft, motorSpeedFactorRight);

// WiFi
WebServer server(80);
const char* ssid = "ESP32_BALANCE";
const char* password = "12345678";

// Web UI
void handleRoot() {
  String page = "<h2>PID Control</h2>";
  page += "<form action='/set'>";
  page += "Kp: <input name='kp'><br>";
  page += "Ki: <input name='ki'><br>";
  page += "Kd: <input name='kd'><br>";
  page += "<input type='submit'></form>";
  server.send(200, "text/html", page);
}

void handleSet() {
  if (server.hasArg("kp")) Kp = server.arg("kp").toFloat();
  if (server.hasArg("ki")) Ki = server.arg("ki").toFloat();
  if (server.hasArg("kd")) Kd = server.arg("kd").toFloat();

  pid.SetTunings(Kp, Ki, Kd);

  server.send(200, "text/html", "Updated!");
}

void setup() {
  Serial.begin(115200);

  // I2C for ESP32
  Wire.begin(21, 22);
  Wire.setClock(400000);

  // MPU init
  mpu.initialize();
  devStatus = mpu.dmpInitialize();

  // offsets (you should calibrate later)
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
  mpu.setZAccelOffset(0);

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);

    pinMode(INTERRUPT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);

    mpuIntStatus = mpu.getIntStatus();
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();

    pid.SetMode(AUTOMATIC);
    pid.SetSampleTime(10);
    pid.SetOutputLimits(-255, 255);

    Serial.println("DMP Ready!");
  } else {
    Serial.print("DMP Init failed: ");
    Serial.println(devStatus);
  }

  // WiFi AP
  WiFi.softAP(ssid, password);
  Serial.println("WiFi Started");

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();

  if (!dmpReady) return;

  while (!mpuInterrupt && fifoCount < packetSize) {
    pid.Compute();
    motorController.move(output, MIN_ABS_SPEED);
  }

  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  fifoCount = mpu.getFIFOCount();

  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    mpu.resetFIFO();
    Serial.println("FIFO overflow!");
  }
  else if (mpuIntStatus & 0x02) {

    while (fifoCount < packetSize)
      fifoCount = mpu.getFIFOCount();

    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    input = ypr[1] * 180 / M_PI + 180;

    Serial.println(input);
  }
}