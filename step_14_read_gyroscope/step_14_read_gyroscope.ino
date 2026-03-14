// STEP 14: Orientation and Acceleration

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
const float ORIENTATION_THRESHOLD = 7.0;

Adafruit_MPU6050 mpu;

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

  Serial.println("Orientation and acceleration demo ready.");
}

void loop() {
  sensors_event_t accelEvent;
  sensors_event_t gyroEvent;
  sensors_event_t tempEvent;

  mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);

  Serial.print("Accel X: ");
  Serial.print(accelEvent.acceleration.x);
  Serial.print("  Y: ");
  Serial.print(accelEvent.acceleration.y);
  Serial.print("  Z: ");
  Serial.println(accelEvent.acceleration.z);

  printOrientation(
    accelEvent.acceleration.x,
    accelEvent.acceleration.y,
    accelEvent.acceleration.z
  );

  Serial.println();
  delay(300);
}

void printOrientation(float x, float y, float z) {
  if (z > ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: flat, face up");
  } else if (z < -ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: flat, face down");
  } else if (x > ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: tilted right");
  } else if (x < -ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: tilted left");
  } else if (y > ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: tip down");
  } else if (y < -ORIENTATION_THRESHOLD) {
    Serial.println("Orientation: tip up");
  } else {
    Serial.println("Orientation: between directions");
  }
}
