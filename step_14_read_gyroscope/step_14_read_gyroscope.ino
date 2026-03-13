// STEP 14: Read Gyroscope Data

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);

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
