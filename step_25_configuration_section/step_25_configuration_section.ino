// STEP 25: Configuration Section

#include <Adafruit_MPU6050.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Configuration values students can tune easily.
const int BUTTON_PIN = D0;
const int BLADE_PIN = D0;
const int LED_COUNT = 60;
const int BLADE_BRIGHTNESS = 80;
const float SWING_THRESHOLD = 4.0;
const int BLADE_RED = 0;
const int BLADE_GREEN = 0;
const int BLADE_BLUE = 180;

bool saberOn = true;

Adafruit_MPU6050 mpu;
Adafruit_NeoPixel strip(LED_COUNT, BLADE_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(BLADE_BRIGHTNESS);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found. Check wiring.");
    while (true) {
      delay(10);
    }
  }
}

void loop() {
  if (saberOn) {
    setBladeColor(BLADE_RED, BLADE_GREEN, BLADE_BLUE);
  }

  if (swingDetected()) {
    Serial.println("Swing detected.");
  }

  delay(50);
}

bool swingDetected() {
  sensors_event_t accelEvent;
  sensors_event_t gyroEvent;
  sensors_event_t tempEvent;

  mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);

  float movement = fabs(gyroEvent.gyro.x) + fabs(gyroEvent.gyro.y) + fabs(gyroEvent.gyro.z);
  return movement > SWING_THRESHOLD;
}

void setBladeColor(int red, int green, int blue) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }

  strip.show();
}
