// STEP 14B: Movement Calibration
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D0  -> MPU6050 SCL
// XIAO ESP32-C3 D10 -> MPU6050 SDA
// XIAO ESP32-C3 3V3 -> MPU6050 VCC
// XIAO ESP32-C3 GND -> MPU6050 GND
//
// The raw gyroscope values are angular velocity in radians per second.
// That means:
// - a positive or negative number shows the direction of rotation
// - a bigger number means the saber is rotating faster
// - values near zero mean the saber is mostly still
//
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

const float ROTATION_THRESHOLD = 1.5;
const AxisName SABER_RIGHT_AXIS = AXIS_X;
const AxisName SABER_TIP_AXIS = AXIS_Y;
const AxisName SABER_TWIST_AXIS = AXIS_Z;
const int SABER_RIGHT_SIGN = -1;
const int SABER_TIP_SIGN = -1;
const int SABER_TWIST_SIGN = 1;

Adafruit_MPU6050 mpu;

float getMappedAxis(float x, float y, float z, AxisName axisName, int axisSign);
void printRawGyro(float x, float y, float z);
void printRotationState(float x, float y, float z);

void setup() {
  Serial.begin(115200);
  Wire.begin(GYRO_SDA_PIN, GYRO_SCL_PIN);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found. Check wiring.");
    while (true) {
      delay(10);
    }
  }

  Serial.println("Gyroscope rotation demo ready.");
  Serial.println("Raw gyro values are in radians per second.");
  Serial.println("Positive and negative values show opposite rotation directions.");
}

void loop() {
  sensors_event_t accelEvent;
  sensors_event_t gyroEvent;
  sensors_event_t tempEvent;

  mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);

  printRawGyro(
    gyroEvent.gyro.x,
    gyroEvent.gyro.y,
    gyroEvent.gyro.z
  );

  printRotationState(
    gyroEvent.gyro.x,
    gyroEvent.gyro.y,
    gyroEvent.gyro.z
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

void printRawGyro(float x, float y, float z) {
  Serial.print("Raw gyro X: ");
  Serial.print(x);
  Serial.print("  Y: ");
  Serial.print(y);
  Serial.print("  Z: ");
  Serial.println(z);
}

void printRotationState(float x, float y, float z) {
  float saberRight = getMappedAxis(x, y, z, SABER_RIGHT_AXIS, SABER_RIGHT_SIGN);
  float saberTip = getMappedAxis(x, y, z, SABER_TIP_AXIS, SABER_TIP_SIGN);
  float saberTwist = getMappedAxis(x, y, z, SABER_TWIST_AXIS, SABER_TWIST_SIGN);

  if (saberTwist > ROTATION_THRESHOLD) {
    Serial.println("Rotation: twisting one way");
  } else if (saberTwist < -ROTATION_THRESHOLD) {
    Serial.println("Rotation: twisting the other way");
  } else if (saberRight > ROTATION_THRESHOLD) {
    Serial.println("Rotation: sweeping right");
  } else if (saberRight < -ROTATION_THRESHOLD) {
    Serial.println("Rotation: sweeping left");
  } else if (saberTip > ROTATION_THRESHOLD) {
    Serial.println("Rotation: tipping down");
  } else if (saberTip < -ROTATION_THRESHOLD) {
    Serial.println("Rotation: tipping up");
  } else {
    Serial.println("Rotation: mostly still");
  }
}

