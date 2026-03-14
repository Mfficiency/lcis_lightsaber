// STEP 13B: Orientation Calibration
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D0  -> MPU6050 SCL
// XIAO ESP32-C3 D10 -> MPU6050 SDA
// XIAO ESP32-C3 3V3 -> MPU6050 VCC
// XIAO ESP32-C3 GND -> MPU6050 GND
//
// This sketch uses the accelerometer to estimate which way the saber is pointing.
// Update the axis mapping below if the MPU6050 is mounted in a different orientation.

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int GYRO_SCL_PIN = 5;
const int GYRO_SDA_PIN = 4;
#else
const int GYRO_SCL_PIN = D0;
const int GYRO_SDA_PIN = D10;
#endif

enum AxisName {
  AXIS_X,
  AXIS_Y,
  AXIS_Z
};

const float ORIENTATION_THRESHOLD = 7.0;
const AxisName SABER_RIGHT_AXIS = AXIS_Y;
const AxisName SABER_TIP_AXIS = AXIS_Z;
const AxisName SABER_UP_AXIS = AXIS_X;
const int SABER_RIGHT_SIGN = -1;
const int SABER_TIP_SIGN = -1;
const int SABER_UP_SIGN = 1;

Adafruit_MPU6050 mpu;

float getMappedAxis(float x, float y, float z, AxisName axisName, int axisSign);
void printRawAcceleration(float x, float y, float z);
void printOrientation(float x, float y, float z);

void setup() {
  Serial.begin(115200);
  Wire.begin(GYRO_SDA_PIN, GYRO_SCL_PIN);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found. Check wiring.");
    while (true) {
      delay(10);
    }
  }

  Serial.println("Orientation demo ready.");
}

void loop() {
  sensors_event_t accelEvent;
  sensors_event_t gyroEvent;
  sensors_event_t tempEvent;

  mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);

  printRawAcceleration(
    accelEvent.acceleration.x,
    accelEvent.acceleration.y,
    accelEvent.acceleration.z
  );

  printOrientation(
    accelEvent.acceleration.x,
    accelEvent.acceleration.y,
    accelEvent.acceleration.z
  );

  Serial.println();
  delay(300);
}

float getMappedAxis(float x, float y, float z, AxisName axisName, int axisSign) {
  float axisValue = 0.0;

  if (axisName == AXIS_X) {
    axisValue = x;
  } else if (axisName == AXIS_Y) {
    axisValue = y;
  } else {
    axisValue = z;
  }

  return axisValue * axisSign;
}

void printRawAcceleration(float x, float y, float z) {
  Serial.print("Raw accel X: ");
  Serial.print(x);
  Serial.print("  Y: ");
  Serial.print(y);
  Serial.print("  Z: ");
  Serial.println(z);
}

void printOrientation(float x, float y, float z) {
  float saberRight = getMappedAxis(x, y, z, SABER_RIGHT_AXIS, SABER_RIGHT_SIGN);
  float saberTip = getMappedAxis(x, y, z, SABER_TIP_AXIS, SABER_TIP_SIGN);
  float saberUp = getMappedAxis(x, y, z, SABER_UP_AXIS, SABER_UP_SIGN);

  if (saberUp > ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: flat, face up");
  } else if (saberUp < -ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: flat, face down");
  } else if (saberRight > ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: tilted right");
  } else if (saberRight < -ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: tilted left");
  } else if (saberTip > ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: tip down");
  } else if (saberTip < -ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: tip up");
  } else {
    Serial.println("Orientation: between directions");
  }
}

