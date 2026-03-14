// STEP 19: Clash Flash Effect

#include <Adafruit_MPU6050.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int LED_STRIP_PIN = 2;
const int GYRO_SCL_PIN = 5;
const int GYRO_SDA_PIN = 4;
#else
const int LED_STRIP_PIN = D9;
const int GYRO_SCL_PIN = D0;
const int GYRO_SDA_PIN = D10;
#endif

const int LED_COUNT = 60;
const int SAFE_BRIGHTNESS = 25;
const float CLASH_THRESHOLD = 10.0;

Adafruit_MPU6050 mpu;
Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  Wire.begin(GYRO_SDA_PIN, GYRO_SCL_PIN);
  strip.begin();
  strip.setBrightness(SAFE_BRIGHTNESS);
  setBladeColor(0, 0, 180);
  Serial.println("Clash flash demo ready.");

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
    Serial.println("Clash detected -> flash");
    clashFlash();
  }

  delay(30);
}

void setBladeColor(int red, int green, int blue) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }

  strip.show();
}

void clashFlash() {
  setBladeColor(255, 255, 255);
  delay(80);
  setBladeColor(0, 0, 180);
}

