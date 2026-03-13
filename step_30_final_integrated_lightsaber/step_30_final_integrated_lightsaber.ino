// STEP 30: Final Integrated Lightsaber

#include <Adafruit_MPU6050.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const int BUTTON_PIN = 0;
const int BLADE_PIN = 2;
const int BUZZER_PIN = 10;
const int LED_COUNT = 60;
const int BLADE_BRIGHTNESS = 80;
const unsigned long ANIMATION_INTERVAL_MS = 25;
const float SWING_THRESHOLD = 4.0;
const float CLASH_THRESHOLD = 10.0;

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
Adafruit_NeoPixel strip(LED_COUNT, BLADE_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);

  strip.begin();
  strip.setBrightness(BLADE_BRIGHTNESS);
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
    if (currentState == OFF_STATE) {
      currentState = TURNING_ON_STATE;
      animationIndex = 0;
      playStartupSound();
    } else if (currentState == ON_STATE) {
      currentState = TURNING_OFF_STATE;
      animationIndex = LED_COUNT;
    }
  }

  lastButtonState = buttonState;
}

void updateStateMachine() {
  unsigned long now = millis();

  if (currentState == TURNING_ON_STATE && now - lastAnimationUpdate >= ANIMATION_INTERVAL_MS) {
    lastAnimationUpdate = now;
    strip.setPixelColor(animationIndex, strip.Color(0, 0, 180));
    strip.show();
    animationIndex++;

    if (animationIndex >= LED_COUNT) {
      currentState = ON_STATE;
    }
  }

  if (currentState == TURNING_OFF_STATE && now - lastAnimationUpdate >= ANIMATION_INTERVAL_MS) {
    lastAnimationUpdate = now;
    animationIndex--;
    strip.setPixelColor(animationIndex, 0);
    strip.show();

    if (animationIndex <= 0) {
      currentState = OFF_STATE;
      strip.clear();
      strip.show();
    }
  }
}

void updateIdleEffect() {
  unsigned long now = millis();
  if (now - lastIdleUpdate < 60) {
    return;
  }

  lastIdleUpdate = now;
  int blueValue = constrain(170 + random(-15, 16), 120, 255);

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, blueValue));
  }

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
  setBladeColor(80, 80, 255);
  tone(BUZZER_PIN, 900, 80);
  delay(60);
}

void clashEffect() {
  setBladeColor(255, 255, 255);
  tone(BUZZER_PIN, 1400, 120);
  delay(90);
}

void playStartupSound() {
  tone(BUZZER_PIN, 440, 80);
  delay(100);
  tone(BUZZER_PIN, 660, 80);
  delay(100);
  tone(BUZZER_PIN, 880, 120);
  delay(140);
}

void setBladeColor(int red, int green, int blue) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }

  strip.show();
}
