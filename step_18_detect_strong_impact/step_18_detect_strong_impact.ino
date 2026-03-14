// STEP 18: Detect Strong Impact

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

Adafruit_MPU6050 mpu;

const float SWING_THRESHOLD = 2.0;
const float CLASH_THRESHOLD = 5.0;

void setup() {
  Serial.begin(115200);
  Wire.begin(GYRO_SDA_PIN, GYRO_SCL_PIN);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found. Check wiring.");
    while (true) {
      delay(10);
    }
  }
}

void loop() {
  sensors_event_t accelEvent;
  sensors_event_t gyroEvent;
  sensors_event_t tempEvent;

  mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);

  float movement = fabs(gyroEvent.gyro.x) + fabs(gyroEvent.gyro.y) + fabs(gyroEvent.gyro.z);

  if (movement > CLASH_THRESHOLD) {
    Serial.println("Strong impact detected.");
  } else if (movement > SWING_THRESHOLD) {
    Serial.println("Swing detected.");
  }

  delay(50);
}
