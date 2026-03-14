// STEP 13: Read Gyroscope Data

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

  Serial.print("Gyro X: ");
  Serial.print(gyroEvent.gyro.x);
  Serial.print("  Y: ");
  Serial.print(gyroEvent.gyro.y);
  Serial.print("  Z: ");
  Serial.println(gyroEvent.gyro.z);

  delay(200);
}
