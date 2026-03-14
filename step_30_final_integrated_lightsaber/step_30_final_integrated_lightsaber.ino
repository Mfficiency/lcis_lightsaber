// STEP 30: Final Integrated Lightsaber
//
// ******************************************************
// WARNING: THIS IS THE ONLY LESSON SKETCH THAT MAY RUN
// ABOVE 10 PERCENT LED BRIGHTNESS.
// USE A PROPER POWER SOURCE FOR THE LED STRIP.
// DO NOT COPY THIS BRIGHTNESS SETTING INTO EARLIER STEPS.
// ******************************************************

#include <Adafruit_MPU6050.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int BUTTON_PIN = 0;
const int LED_STRIP_PIN = 2;
const int BUZZER_PIN = 10;
const int GYRO_SCL_PIN = 5;
const int GYRO_SDA_PIN = 4;
#else
const int BUTTON_PIN = D7;
const int LED_STRIP_PIN = D8;
const int BUZZER_PIN = D5;
const int GYRO_SCL_PIN = D0;
const int GYRO_SDA_PIN = D10;
#endif

const int LED_COUNT = 60;
const int SAFE_BRIGHTNESS = 25;
const int SHUTDOWN_PEAK_BRIGHTNESS = 60;
const unsigned long ANIMATION_INTERVAL_MS = 25;
const float SWING_THRESHOLD = 4.0;
const float CLASH_THRESHOLD = 10.0;
const int BLADE_RED = 0;
const int BLADE_GREEN = 100;
const int BLADE_BLUE = 0;
const int BLADE_WHITE = 100;

enum SaberState {
  OFF_STATE,
  TURNING_ON_STATE,
  ON_STATE,
  TURNING_OFF_STATE
};

SaberState currentState = OFF_STATE;
int lastButtonState = HIGH;
int animationIndex = 0;
unsigned long lastAnimationUpdate = 0;
unsigned long lastIdleUpdate = 0;

Adafruit_MPU6050 mpu;
Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRBW + NEO_KHZ800);

void pulseBrightness();
void playShutdownSound();
void fillBladeColor(int red, int green, int blue, int white);
void showClashSparkEffect();
void playClashScrapeSound();

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  Wire.begin(GYRO_SDA_PIN, GYRO_SCL_PIN);
  Serial.println("Final lightsaber ready. State -> OFF");
  Serial.println("WARNING: this sketch can drive the strip above 10 percent brightness.");
  Serial.println("WARNING: use proper external LED power and common ground.");

  strip.begin();
  strip.setBrightness(SAFE_BRIGHTNESS);
  strip.clear();
  strip.show();

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found. Check wiring.");
    while (true) {
      delay(10);
    }
  }
}

void loop() {
  handleButton();
  updateStateMachine();

  if (currentState == ON_STATE) {
    updateIdleEffect();
    handleMotionEffects();
  }
}

void handleButton() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    Serial.println("Button pressed");
    if (currentState == OFF_STATE) {
      currentState = TURNING_ON_STATE;
      animationIndex = 0;
      lastAnimationUpdate = 0;
      strip.setBrightness(SAFE_BRIGHTNESS);
      strip.clear();
      strip.show();
      Serial.println("State -> TURNING_ON");
      playStartupSound();
    } else if (currentState == ON_STATE) {
      currentState = TURNING_OFF_STATE;
      animationIndex = 0;
      lastAnimationUpdate = 0;
      Serial.println("State -> TURNING_OFF");
      playShutdownSound();
      pulseBrightness();
    }
  }

  lastButtonState = buttonState;
}

void updateStateMachine() {
  unsigned long now = millis();

  if (currentState == TURNING_ON_STATE && now - lastAnimationUpdate >= ANIMATION_INTERVAL_MS) {
    int leftIndex = animationIndex;
    int rightIndex = LED_COUNT - 1 - animationIndex;

    lastAnimationUpdate = now;
    strip.setPixelColor(leftIndex, strip.Color(BLADE_RED, BLADE_GREEN, BLADE_BLUE, BLADE_WHITE));

    if (rightIndex != leftIndex) {
      strip.setPixelColor(rightIndex, strip.Color(BLADE_RED, BLADE_GREEN, BLADE_BLUE, BLADE_WHITE));
    }

    strip.show();
    animationIndex++;

    if (leftIndex >= rightIndex) {
      pulseBrightness();
      currentState = ON_STATE;
      Serial.println("State -> ON");
    }
  }

  if (currentState == TURNING_OFF_STATE && now - lastAnimationUpdate >= ANIMATION_INTERVAL_MS) {
    int leftIndex = ((LED_COUNT - 1) / 2) - animationIndex;
    int rightIndex = (LED_COUNT / 2) + animationIndex;

    lastAnimationUpdate = now;

    if (leftIndex >= 0) {
      strip.setPixelColor(leftIndex, 0);
    }

    if (rightIndex < LED_COUNT && rightIndex != leftIndex) {
      strip.setPixelColor(rightIndex, 0);
    }

    strip.show();
    animationIndex++;

    if (leftIndex < 0 && rightIndex >= LED_COUNT) {
      currentState = OFF_STATE;
      strip.setBrightness(SAFE_BRIGHTNESS);
      strip.clear();
      strip.show();
      Serial.println("State -> OFF");
    }
  }
}

void updateIdleEffect() {
  unsigned long now = millis();
  if (now - lastIdleUpdate < 60) {
    return;
  }

  lastIdleUpdate = now;
  int greenOffset = random(-12, 13);
  int whiteOffset = random(-10, 11);
  int greenValue = constrain(BLADE_GREEN + greenOffset, 70, 130);
  int whiteValue = constrain(BLADE_WHITE + whiteOffset, 70, 130);

  fillBladeColor(BLADE_RED, greenValue, BLADE_BLUE, whiteValue);

  strip.show();
}

void handleMotionEffects() {
  sensors_event_t accelEvent;
  sensors_event_t gyroEvent;
  sensors_event_t tempEvent;

  mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);

  float movement = fabs(gyroEvent.gyro.x) + fabs(gyroEvent.gyro.y) + fabs(gyroEvent.gyro.z);

  if (movement > CLASH_THRESHOLD) {
    clashEffect();
  } else if (movement > SWING_THRESHOLD) {
    swingEffect();
  }
}

void swingEffect() {
  Serial.println("Swing detected -> blue tint");
  fillBladeColor(0, 90, 35, 110);
  strip.show();
  delay(120);
}

void clashEffect() {
  Serial.println("Clash detected -> spark effect and sound");
  showClashSparkEffect();
  playClashScrapeSound();
  delay(360);
}

void playStartupSound() {
  Serial.println("Startup sound");
  tone(BUZZER_PIN, 440, 80);
  delay(100);
  tone(BUZZER_PIN, 660, 80);
  delay(100);
  tone(BUZZER_PIN, 880, 120);
  delay(140);
}

void playShutdownSound() {
  Serial.println("Shutdown sound");
  tone(BUZZER_PIN, 880, 70);
  delay(90);
  tone(BUZZER_PIN, 660, 70);
  delay(90);
  tone(BUZZER_PIN, 440, 120);
  delay(140);
}

void fillBladeColor(int red, int green, int blue, int white) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue, white));
  }
}

void showClashSparkEffect() {
  fillBladeColor(70, 15, 0, 20);

  for (int i = 0; i < 14; i++) {
    int sparkIndex = random(LED_COUNT);
    int redValue = random(180, 256);
    int whiteValue = random(120, 256);
    strip.setPixelColor(sparkIndex, strip.Color(redValue, 30, 0, whiteValue));
  }

  strip.show();
}

void playClashScrapeSound() {
  tone(BUZZER_PIN, 2200, 18);
  delay(22);
  tone(BUZZER_PIN, 1700, 22);
  delay(26);
  tone(BUZZER_PIN, 2100, 14);
  delay(18);
  tone(BUZZER_PIN, 1500, 28);
  delay(32);
  tone(BUZZER_PIN, 1800, 16);
  delay(20);
  tone(BUZZER_PIN, 1200, 34);
  delay(40);
}

void pulseBrightness() {
  for (int brightness = SAFE_BRIGHTNESS; brightness <= SHUTDOWN_PEAK_BRIGHTNESS; brightness += 5) {
    strip.setBrightness(brightness);
    strip.show();
    delay(15);
  }

  for (int brightness = SHUTDOWN_PEAK_BRIGHTNESS; brightness >= SAFE_BRIGHTNESS; brightness -= 5) {
    strip.setBrightness(brightness);
    strip.show();
    delay(35);
  }
}

