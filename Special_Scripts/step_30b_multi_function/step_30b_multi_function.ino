// SPECIAL: Step 30 Multi-Function
//
// Builds on step_30_final_integrated_lightsaber.
//
// CONTROLS
// Short press:
// - OFF -> startup
// - ON -> next mode
//
// Double press:
// - ON -> shutdown
//
// Count mode:
// - hold button to count up
// - release to count down
//
// MODES
// 1. Default lightsaber mode
// 2. Color choose mode using orientation
// 3. Nightlight mode using orientation
// 4. Count mode

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
const unsigned long IDLE_INTERVAL_MS = 60;
const unsigned long ORIENTATION_UPDATE_MS = 120;
const unsigned long DOUBLE_PRESS_MS = 350;
const unsigned long COUNT_HOLD_START_MS = 250;
const unsigned long COUNT_UP_INTERVAL_MS = 500;
const unsigned long COUNT_DOWN_INTERVAL_MS = 2000;
const float SWING_THRESHOLD = 4.0;
const float CLASH_THRESHOLD = 10.0;
const float GRAVITY_REFERENCE = 9.8;
const float NIGHTLIGHT_SMOOTHING = 0.18;

enum SaberState {
  OFF_STATE,
  TURNING_ON_STATE,
  ON_STATE,
  TURNING_OFF_STATE
};

enum SaberMode {
  DEFAULT_MODE,
  COLOR_CHOOSE_MODE,
  NIGHTLIGHT_MODE,
  COUNT_MODE
};

enum CountModeState {
  COUNT_IDLE_STATE,
  COUNTING_UP_STATE,
  COUNTING_DOWN_STATE
};

enum AxisName {
  AXIS_X,
  AXIS_Y,
  AXIS_Z
};

const AxisName SABER_RIGHT_AXIS = AXIS_Y;
const AxisName SABER_TIP_AXIS = AXIS_Z;
const AxisName SABER_UP_AXIS = AXIS_X;
const int SABER_RIGHT_SIGN = -1;
const int SABER_TIP_SIGN = -1;
const int SABER_UP_SIGN = 1;

SaberState currentState = OFF_STATE;
SaberMode currentMode = DEFAULT_MODE;
CountModeState countModeState = COUNT_IDLE_STATE;

int bladeRed = 0;
int bladeGreen = 100;
int bladeBlue = 0;
int bladeWhite = 100;

int lastButtonState = HIGH;
int animationIndex = 0;
int countModeLitCount = 0;
unsigned long lastAnimationUpdate = 0;
unsigned long lastIdleUpdate = 0;
unsigned long lastOrientationUpdate = 0;
unsigned long lastCountModeUpdate = 0;
unsigned long buttonPressStart = 0;
unsigned long lastReleaseTime = 0;
bool pendingShortPress = false;
bool countHoldActive = false;
float smoothedNightLightWhite = 5.0;
float smoothedNightLightLitCount = LED_COUNT;

Adafruit_MPU6050 mpu;
Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRBW + NEO_KHZ800);

void handleButton();
void processPendingShortPress();
void handleShortPress();
void handleDoublePress();
void cycleMode();
void showCurrentMode();
void updateStateMachine();
void startTurnOn();
void startTurnOff();
void updateDefaultMode();
void updateColorChooseMode();
void updateNightLightMode();
void updateCountMode();
void handleMotionEffects();
void swingEffect();
void clashEffect();
void playStartupSound();
void playShutdownSound();
void playModeCycleSound();
void playClashScrapeSound();
void pulseBrightness();
void fillBladeColor(int red, int green, int blue, int white);
void fillCurrentBladeColor();
void showClashSparkEffect();
void showNightLightBlade(int whiteValue, int litCount);
void showCountModeIdle();
float clamp01(float value);
float getMappedAxis(float x, float y, float z, AxisName axisName, int axisSign);
void readOrientation(float& saberRight, float& saberTip, float& saberUp);
void applyColorFromOrientation(float saberRight, float saberTip, float saberUp);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  Wire.begin(GYRO_SDA_PIN, GYRO_SCL_PIN);
  Serial.println("Step 30 multi-function ready. State -> OFF");
  Serial.println("Short press powers on and cycles modes. Double press powers off.");

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

  showCurrentMode();
}

void loop() {
  handleButton();
  processPendingShortPress();
  updateStateMachine();

  if (currentState != ON_STATE) {
    return;
  }

  if (currentMode == DEFAULT_MODE) {
    updateDefaultMode();
  } else if (currentMode == COLOR_CHOOSE_MODE) {
    updateColorChooseMode();
  } else if (currentMode == NIGHTLIGHT_MODE) {
    updateNightLightMode();
  } else {
    updateCountMode();
  }
}

void handleButton() {
  int buttonState = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  if (buttonState == LOW && lastButtonState == HIGH) {
    buttonPressStart = now;
    countHoldActive = false;
  }

  if (buttonState == LOW && currentState == ON_STATE && currentMode == COUNT_MODE && !countHoldActive) {
    if (now - buttonPressStart >= COUNT_HOLD_START_MS) {
      pendingShortPress = false;
      countHoldActive = true;
      countModeState = COUNTING_UP_STATE;
      lastCountModeUpdate = now;
      Serial.println("Count mode -> counting up");
    }
  }

  if (buttonState == HIGH && lastButtonState == LOW) {
    if (countHoldActive) {
      countHoldActive = false;
      if (currentState == ON_STATE && currentMode == COUNT_MODE) {
        countModeState = COUNTING_DOWN_STATE;
        lastCountModeUpdate = now;
        Serial.println("Count mode -> counting down");
      }
    } else if (currentState == ON_STATE) {
      if (pendingShortPress && now - lastReleaseTime <= DOUBLE_PRESS_MS) {
        pendingShortPress = false;
        handleDoublePress();
      } else {
        pendingShortPress = true;
        lastReleaseTime = now;
      }
    } else {
      handleShortPress();
    }
  }

  lastButtonState = buttonState;
}

void processPendingShortPress() {
  if (!pendingShortPress) {
    return;
  }

  if (millis() - lastReleaseTime > DOUBLE_PRESS_MS) {
    pendingShortPress = false;
    handleShortPress();
  }
}

void handleShortPress() {
  Serial.println("Short press");

  if (currentState == OFF_STATE) {
    startTurnOn();
  } else if (currentState == ON_STATE) {
    cycleMode();
  }
}

void handleDoublePress() {
  Serial.println("Double press");

  if (currentState == ON_STATE) {
    startTurnOff();
  }
}

void cycleMode() {
  currentMode = static_cast<SaberMode>((currentMode + 1) % 4);
  playModeCycleSound();
  showCurrentMode();
  lastIdleUpdate = 0;
  lastOrientationUpdate = 0;
  smoothedNightLightWhite = 5.0;
  smoothedNightLightLitCount = LED_COUNT;
  countModeState = COUNT_IDLE_STATE;
  countModeLitCount = 0;
  countHoldActive = false;

  if (currentState != ON_STATE) {
    return;
  }

  if (currentMode == DEFAULT_MODE || currentMode == COLOR_CHOOSE_MODE) {
    fillCurrentBladeColor();
    strip.show();
  } else if (currentMode == COUNT_MODE) {
    showCountModeIdle();
    strip.show();
  }
}

void showCurrentMode() {
  if (currentMode == DEFAULT_MODE) {
    Serial.println("Mode -> Default lightsaber");
  } else if (currentMode == COLOR_CHOOSE_MODE) {
    Serial.println("Mode -> Choose color with orientation");
  } else if (currentMode == NIGHTLIGHT_MODE) {
    Serial.println("Mode -> Nightlight");
  } else {
    Serial.println("Mode -> Count mode");
  }
}

void startTurnOn() {
  currentState = TURNING_ON_STATE;
  animationIndex = 0;
  lastAnimationUpdate = 0;
  pendingShortPress = false;
  countHoldActive = false;
  strip.setBrightness(SAFE_BRIGHTNESS);
  strip.clear();
  strip.show();
  Serial.println("State -> TURNING_ON");
  playStartupSound();
}

void startTurnOff() {
  currentState = TURNING_OFF_STATE;
  animationIndex = 0;
  lastAnimationUpdate = 0;
  pendingShortPress = false;
  countHoldActive = false;
  countModeState = COUNT_IDLE_STATE;
  Serial.println("State -> TURNING_OFF");
  playShutdownSound();
  pulseBrightness();
}

void updateStateMachine() {
  unsigned long now = millis();

  if (currentState == TURNING_ON_STATE && now - lastAnimationUpdate >= ANIMATION_INTERVAL_MS) {
    int leftIndex = animationIndex;
    int rightIndex = LED_COUNT - 1 - animationIndex;

    lastAnimationUpdate = now;
    strip.setPixelColor(leftIndex, strip.Color(bladeRed, bladeGreen, bladeBlue, bladeWhite));

    if (rightIndex != leftIndex) {
      strip.setPixelColor(rightIndex, strip.Color(bladeRed, bladeGreen, bladeBlue, bladeWhite));
    }

    strip.show();
    animationIndex++;

    if (leftIndex >= rightIndex) {
      pulseBrightness();
      currentState = ON_STATE;
      lastIdleUpdate = 0;
      lastOrientationUpdate = 0;
      lastCountModeUpdate = 0;
      countModeLitCount = 0;
      countModeState = COUNT_IDLE_STATE;
      smoothedNightLightWhite = 5.0;
      smoothedNightLightLitCount = LED_COUNT;

      if (currentMode == COUNT_MODE) {
        showCountModeIdle();
        strip.show();
      }

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
      countModeState = COUNT_IDLE_STATE;
      countModeLitCount = 0;
      Serial.println("State -> OFF");
    }
  }
}

void updateDefaultMode() {
  unsigned long now = millis();
  if (now - lastIdleUpdate < IDLE_INTERVAL_MS) {
    handleMotionEffects();
    return;
  }

  lastIdleUpdate = now;
  int whiteValue = constrain(bladeWhite + random(0, 21), 0, 255);
  fillBladeColor(bladeRed, bladeGreen, bladeBlue, whiteValue);
  strip.show();
  handleMotionEffects();
}

void updateColorChooseMode() {
  unsigned long now = millis();
  if (now - lastOrientationUpdate < ORIENTATION_UPDATE_MS) {
    return;
  }

  float saberRight = 0.0;
  float saberTip = 0.0;
  float saberUp = 0.0;
  readOrientation(saberRight, saberTip, saberUp);
  applyColorFromOrientation(saberRight, saberTip, saberUp);

  lastOrientationUpdate = now;
  fillCurrentBladeColor();
  strip.show();
}

void updateNightLightMode() {
  unsigned long now = millis();
  if (now - lastOrientationUpdate < ORIENTATION_UPDATE_MS) {
    return;
  }

  float saberRight = 0.0;
  float saberTip = 0.0;
  float saberUp = 0.0;
  readOrientation(saberRight, saberTip, saberUp);

  lastOrientationUpdate = now;

  float tipUpAmount = clamp01(-saberTip / GRAVITY_REFERENCE);
  float tipDownAmount = clamp01(saberTip / GRAVITY_REFERENCE);
  float uprightAmount = clamp01(fabs(saberTip) / GRAVITY_REFERENCE);

  int whiteValue = 5 + static_cast<int>(145.0 * uprightAmount);
  int litCount = LED_COUNT;

  if (tipDownAmount > 0.01) {
    litCount = 1 + static_cast<int>((LED_COUNT - 1) * (1.0 - tipDownAmount));
  }

  if (tipUpAmount > 0.95) {
    whiteValue = 150;
  }

  smoothedNightLightWhite += (whiteValue - smoothedNightLightWhite) * NIGHTLIGHT_SMOOTHING;
  smoothedNightLightLitCount += (litCount - smoothedNightLightLitCount) * NIGHTLIGHT_SMOOTHING;

  int stableWhiteValue = static_cast<int>((smoothedNightLightWhite + 2.5) / 5.0) * 5;
  int stableLitCount = static_cast<int>(smoothedNightLightLitCount + 0.5);

  stableWhiteValue = constrain(stableWhiteValue, 5, 150);
  stableLitCount = constrain(stableLitCount, 1, LED_COUNT);

  showNightLightBlade(stableWhiteValue, stableLitCount);
  strip.show();
}

void updateCountMode() {
  unsigned long now = millis();

  if (countModeState == COUNT_IDLE_STATE) {
    showCountModeIdle();
    strip.show();
    return;
  }

  unsigned long intervalMs = COUNT_UP_INTERVAL_MS;
  if (countModeState == COUNTING_DOWN_STATE) {
    intervalMs = COUNT_DOWN_INTERVAL_MS;
  }

  if (now - lastCountModeUpdate < intervalMs) {
    return;
  }

  lastCountModeUpdate = now;

  if (countModeState == COUNTING_UP_STATE) {
    if (countModeLitCount < LED_COUNT) {
      countModeLitCount++;
    }
  } else if (countModeState == COUNTING_DOWN_STATE) {
    if (countModeLitCount > 0) {
      countModeLitCount--;
    } else {
      countModeState = COUNT_IDLE_STATE;
    }
  }

  if (countModeState == COUNT_IDLE_STATE && countModeLitCount == 0) {
    showCountModeIdle();
    strip.show();
    return;
  }

  strip.clear();
  for (int i = 0; i < countModeLitCount; i++) {
    strip.setPixelColor(i, strip.Color(bladeRed, bladeGreen, bladeBlue, bladeWhite));
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
  Serial.println("Swing detected -> blue tint");
  fillBladeColor(
    constrain(bladeRed / 2, 0, 255),
    constrain(bladeGreen - 10, 0, 255),
    constrain(bladeBlue + 35, 0, 255),
    constrain(bladeWhite + 10, 0, 255)
  );
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
  tone(BUZZER_PIN, 440, 80);
  delay(100);
  tone(BUZZER_PIN, 660, 80);
  delay(100);
  tone(BUZZER_PIN, 880, 120);
  delay(140);
}

void playShutdownSound() {
  tone(BUZZER_PIN, 880, 70);
  delay(90);
  tone(BUZZER_PIN, 660, 70);
  delay(90);
  tone(BUZZER_PIN, 440, 120);
  delay(140);
}

void playModeCycleSound() {
  if (currentMode == DEFAULT_MODE) {
    tone(BUZZER_PIN, 700, 55);
    delay(70);
  } else if (currentMode == COLOR_CHOOSE_MODE) {
    tone(BUZZER_PIN, 900, 55);
    delay(70);
  } else if (currentMode == NIGHTLIGHT_MODE) {
    tone(BUZZER_PIN, 1100, 55);
    delay(70);
  } else {
    tone(BUZZER_PIN, 1300, 55);
    delay(70);
  }
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

void fillBladeColor(int red, int green, int blue, int white) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue, white));
  }
}

void fillCurrentBladeColor() {
  fillBladeColor(bladeRed, bladeGreen, bladeBlue, bladeWhite);
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

void showNightLightBlade(int whiteValue, int litCount) {
  strip.clear();

  int startIndex = (LED_COUNT - litCount) / 2;
  int endIndex = startIndex + litCount - 1;

  for (int i = startIndex; i <= endIndex; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0, whiteValue));
  }
}

void showCountModeIdle() {
  strip.clear();

  for (int i = 4; i < LED_COUNT; i += 10) {
    strip.setPixelColor(i, strip.Color(5, 0, 0, 0));
  }

  for (int i = 9; i < LED_COUNT; i += 10) {
    strip.setPixelColor(i, strip.Color(15, 2, 0, 0));
  }
}

float clamp01(float value) {
  if (value < 0.0) {
    return 0.0;
  }

  if (value > 1.0) {
    return 1.0;
  }

  return value;
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

void readOrientation(float& saberRight, float& saberTip, float& saberUp) {
  sensors_event_t accelEvent;
  sensors_event_t gyroEvent;
  sensors_event_t tempEvent;

  mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);

  saberRight = getMappedAxis(
    accelEvent.acceleration.x,
    accelEvent.acceleration.y,
    accelEvent.acceleration.z,
    SABER_RIGHT_AXIS,
    SABER_RIGHT_SIGN
  );

  saberTip = getMappedAxis(
    accelEvent.acceleration.x,
    accelEvent.acceleration.y,
    accelEvent.acceleration.z,
    SABER_TIP_AXIS,
    SABER_TIP_SIGN
  );

  saberUp = getMappedAxis(
    accelEvent.acceleration.x,
    accelEvent.acceleration.y,
    accelEvent.acceleration.z,
    SABER_UP_AXIS,
    SABER_UP_SIGN
  );
}

void applyColorFromOrientation(float saberRight, float saberTip, float saberUp) {
  float rightPositive = clamp01(saberRight / GRAVITY_REFERENCE);
  float rightNegative = clamp01(-saberRight / GRAVITY_REFERENCE);
  float tipPositive = clamp01(saberTip / GRAVITY_REFERENCE);
  float tipNegative = clamp01(-saberTip / GRAVITY_REFERENCE);
  float upPositive = clamp01(saberUp / GRAVITY_REFERENCE);
  float upNegative = clamp01(-saberUp / GRAVITY_REFERENCE);

  bladeRed = static_cast<int>(255.0 * clamp01(rightPositive * 0.75 + tipPositive * 0.45));
  bladeGreen = static_cast<int>(255.0 * clamp01(upPositive * 0.85 + rightPositive * 0.15));
  bladeBlue = static_cast<int>(255.0 * clamp01(rightNegative * 0.75 + tipNegative * 0.55));
  bladeWhite = static_cast<int>(180.0 * clamp01(upNegative * 0.9 + fabs(saberRight) / (GRAVITY_REFERENCE * 5.0)));

  if (bladeRed < 20 && bladeGreen < 20 && bladeBlue < 20) {
    bladeGreen = 40;
    bladeWhite = 40;
  }
}
