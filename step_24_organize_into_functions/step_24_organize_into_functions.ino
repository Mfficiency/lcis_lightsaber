// STEP 24: Organize Code into Functions

#include <Adafruit_MPU6050.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int BUTTON_PIN = 0;
const int LED_STRIP_PIN = 2;
const int GYRO_SCL_PIN = 5;
const int GYRO_SDA_PIN = 4;
#else
const int BUTTON_PIN = D7;
const int LED_STRIP_PIN = D9;
const int GYRO_SCL_PIN = D0;
const int GYRO_SDA_PIN = D10;
#endif

const int LED_COUNT = 60;
const float SWING_THRESHOLD = 4.0;

bool saberOn = false;
bool bladeIsLit = false;
int lastButtonState = HIGH;

Adafruit_MPU6050 mpu;
Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  setupButton();
  setupBlade();
  setupGyro();
}

void loop() {
  readButton();
  updateBlade();

  if (saberOn && swingDetected()) {
    flashBlade();
  }

  delay(20);
}

void setupButton() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void setupBlade() {
  strip.begin();
  strip.clear();
  strip.show();
}

void setupGyro() {
  Serial.begin(115200);
  Wire.begin(GYRO_SDA_PIN, GYRO_SCL_PIN);
  Serial.println("Organized lightsaber sketch ready.");

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found. Check wiring.");
    while (true) {
      delay(10);
    }
  }
}

void readButton() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    saberOn = !saberOn;
    Serial.println("Button pressed");
  }

  lastButtonState = buttonState;
}

void updateBlade() {
  if (saberOn) {
    if (!bladeIsLit) {
      Serial.println("Led Strip ON");
      bladeIsLit = true;
    }
    setBladeColor(0, 0, 180);
  } else {
    if (bladeIsLit) {
      Serial.println("Led Strip OFF");
      bladeIsLit = false;
    }
    strip.clear();
    strip.show();
  }
}

bool swingDetected() {
  sensors_event_t accelEvent;
  sensors_event_t gyroEvent;
  sensors_event_t tempEvent;

  mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);

  float movement = fabs(gyroEvent.gyro.x) + fabs(gyroEvent.gyro.y) + fabs(gyroEvent.gyro.z);
  return movement > SWING_THRESHOLD;
}

void flashBlade() {
  Serial.println("Swing detected -> Led Strip flash");
  setBladeColor(80, 80, 255);
  delay(80);
  setBladeColor(0, 0, 180);
}

void setBladeColor(int red, int green, int blue) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }

  strip.show();
}
