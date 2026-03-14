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
// - hold button to choose minutes
// - release to start the timer
//
// MODES
// 1. Default lightsaber mode
// 2. Color choose mode using orientation
// 3. Nightlight mode using orientation
// 4. Count mode
// 5. Sweeping rainbow
// 6. Spirit level
// 7. Off


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
const int SAFE_BRIGHTNESS = 100;
const int SHUTDOWN_PEAK_BRIGHTNESS = 60;
const int SABER_BRIGHTNESS = 80;
const int COLOR_CHOOSE_BRIGHTNESS = 80;
const int NIGHTLIGHT_BRIGHTNESS = 35;
const int COUNT_MODE_BRIGHTNESS = 70;
const int RAINBOW_BRIGHTNESS = 90;
const int SPIRIT_LEVEL_BRIGHTNESS = 40;
const int OFF_MODE_BRIGHTNESS = 0;
const unsigned long ANIMATION_INTERVAL_MS = 25;
const unsigned long IDLE_INTERVAL_MS = 60;
const unsigned long ORIENTATION_UPDATE_MS = 120;
const unsigned long DOUBLE_PRESS_MS = 220;
const unsigned long COUNT_HOLD_START_MS = 250;
const unsigned long SPIRIT_LEVEL_HOLD_MS = 400;
const unsigned long COUNT_UP_INTERVAL_MS = 700;
const unsigned long COUNT_DOWN_INTERVAL_MS = 1000;
const unsigned long RAINBOW_UPDATE_MS = 40;
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
  COUNT_MODE,
  RAINBOW_MODE,
  SPIRIT_LEVEL_MODE,
  OFF_MODE
};

enum CountModeState {
  COUNT_IDLE_STATE,
  COUNT_SELECTING_STATE,
  COUNT_RUNNING_STATE
};

enum RainbowPaletteMode {
  RAINBOW_ALL_COLORS,
  RAINBOW_REDDISH,
  RAINBOW_BLUISH,
  RAINBOW_GREENISH
};

enum AxisName {
  AXIS_X,
  AXIS_Y,
  AXIS_Z
};

enum SpiritLevelAxisMode {
  SPIRIT_LEVEL_AXIS_RIGHT,
  SPIRIT_LEVEL_AXIS_TIP,
  SPIRIT_LEVEL_AXIS_UP
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
int countModeSelectedMinutes = 0;
int countModeMinutesRemaining = 0;
int countModeWhiteLedCount = 0;
unsigned long lastAnimationUpdate = 0;
unsigned long lastIdleUpdate = 0;
unsigned long lastOrientationUpdate = 0;
unsigned long lastCountModeUpdate = 0;
unsigned long lastRainbowUpdate = 0;
unsigned long buttonPressStart = 0;
unsigned long lastReleaseTime = 0;
bool pendingShortPress = false;
bool countHoldActive = false;
bool spiritLevelHoldActive = false;
float smoothedNightLightWhite = 5.0;
float smoothedNightLightLitCount = LED_COUNT;
float rainbowTemperature = 0.5;
uint16_t rainbowOffset = 0;
RainbowPaletteMode rainbowPaletteMode = RAINBOW_ALL_COLORS;
unsigned long lastRainbowSwingTime = 0;
SpiritLevelAxisMode spiritLevelAxisMode = SPIRIT_LEVEL_AXIS_RIGHT;

Adafruit_MPU6050 mpu;
Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRBW + NEO_KHZ800);

void handleButton();
void processPendingShortPress();
void handleShortPress();
void handleDoublePress();
void cycleMode();
void showCurrentMode();
void applyModeBrightness();
void updateStateMachine();
void startTurnOn();
void startTurnOff();
void updateDefaultMode();
void updateColorChooseMode();
void updateNightLightMode();
void updateCountMode();
void updateRainbowMode();
void updateSpiritLevelMode();
void cycleSpiritLevelAxis();
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
void renderCountMode(int greenLedCount, int whiteLedCount);
void playCountdownFinishedSound();
uint32_t colorWheel(byte wheelPos, int whiteOffset);
void showSpiritLevel(float saberRight);
void cycleRainbowPalette();
byte getRainbowPaletteWheelPos(byte wheelPos);
void getCountModeBaseColor(int pixelIndex, int& redValue, int& greenValue, int& blueValue, int& whiteValue);
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
  } else if (currentMode == COUNT_MODE) {
    updateCountMode();
  } else if (currentMode == RAINBOW_MODE) {
    updateRainbowMode();
  } else if (currentMode == SPIRIT_LEVEL_MODE) {
    updateSpiritLevelMode();
  } else {
    strip.clear();
    strip.show();
  }
}

void handleButton() {
  int buttonState = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  if (buttonState == LOW && lastButtonState == HIGH) {
    buttonPressStart = now;
    countHoldActive = false;
    spiritLevelHoldActive = false;
  }

  if (buttonState == LOW && currentState == ON_STATE && currentMode == COUNT_MODE && !countHoldActive) {
    if (now - buttonPressStart >= COUNT_HOLD_START_MS) {
      bool resetRunningCountdown = (countModeState == COUNT_RUNNING_STATE);
      pendingShortPress = false;
      countHoldActive = true;
      if (resetRunningCountdown) {
        countModeSelectedMinutes = 0;
        countModeMinutesRemaining = 0;
        countModeWhiteLedCount = 0;
      }
      countModeState = COUNT_SELECTING_STATE;
      if (!resetRunningCountdown && countModeSelectedMinutes <= 0) {
        countModeSelectedMinutes = 1;
      }
      lastCountModeUpdate = now;
      Serial.println("Count mode -> selecting minutes");
    }
  }

  if (buttonState == LOW && currentState == ON_STATE && currentMode == SPIRIT_LEVEL_MODE && !spiritLevelHoldActive) {
    if (now - buttonPressStart >= SPIRIT_LEVEL_HOLD_MS) {
      pendingShortPress = false;
      spiritLevelHoldActive = true;
      cycleSpiritLevelAxis();
    }
  }

  if (buttonState == HIGH && lastButtonState == LOW) {
    if (countHoldActive) {
      countHoldActive = false;
      if (currentState == ON_STATE && currentMode == COUNT_MODE) {
        if (countModeSelectedMinutes > 0) {
          countModeState = COUNT_RUNNING_STATE;
          countModeMinutesRemaining = countModeSelectedMinutes;
          countModeWhiteLedCount = LED_COUNT;
          lastCountModeUpdate = now;
          Serial.println("Count mode -> timer started");
        } else {
          countModeState = COUNT_IDLE_STATE;
        }
      }
    } else if (spiritLevelHoldActive) {
      spiritLevelHoldActive = false;
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
  currentMode = static_cast<SaberMode>((currentMode + 1) % 7);
  playModeCycleSound();
  showCurrentMode();
  applyModeBrightness();
  lastIdleUpdate = 0;
  lastOrientationUpdate = 0;
  lastRainbowUpdate = 0;
  smoothedNightLightWhite = 5.0;
  smoothedNightLightLitCount = LED_COUNT;
  countModeState = COUNT_IDLE_STATE;
  countModeSelectedMinutes = 0;
  countModeMinutesRemaining = 0;
  countModeWhiteLedCount = 0;
  countHoldActive = false;
  lastRainbowSwingTime = 0;

  if (currentState != ON_STATE) {
    return;
  }

  if (currentMode == DEFAULT_MODE || currentMode == COLOR_CHOOSE_MODE) {
    fillCurrentBladeColor();
    strip.show();
  } else if (currentMode == COUNT_MODE) {
    showCountModeIdle();
    strip.show();
  } else if (currentMode == OFF_MODE) {
    strip.clear();
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
  } else if (currentMode == COUNT_MODE) {
    Serial.println("Mode -> Count mode");
  } else if (currentMode == RAINBOW_MODE) {
    Serial.println("Mode -> Sweeping rainbow");
  } else if (currentMode == SPIRIT_LEVEL_MODE) {
    Serial.println("Mode -> Spirit level");
  } else {
    Serial.println("Mode -> Off");
  }
}

void applyModeBrightness() {
  if (currentMode == DEFAULT_MODE) {
    strip.setBrightness(SABER_BRIGHTNESS);
  } else if (currentMode == COLOR_CHOOSE_MODE) {
    strip.setBrightness(COLOR_CHOOSE_BRIGHTNESS);
  } else if (currentMode == NIGHTLIGHT_MODE) {
    strip.setBrightness(NIGHTLIGHT_BRIGHTNESS);
  } else if (currentMode == COUNT_MODE) {
    strip.setBrightness(COUNT_MODE_BRIGHTNESS);
  } else if (currentMode == RAINBOW_MODE) {
    strip.setBrightness(RAINBOW_BRIGHTNESS);
  } else if (currentMode == SPIRIT_LEVEL_MODE) {
    strip.setBrightness(SPIRIT_LEVEL_BRIGHTNESS);
  } else {
    strip.setBrightness(OFF_MODE_BRIGHTNESS);
  }
}

void startTurnOn() {
  currentState = TURNING_ON_STATE;
  animationIndex = 0;
  lastAnimationUpdate = 0;
  pendingShortPress = false;
  countHoldActive = false;
  applyModeBrightness();
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
  countModeSelectedMinutes = 0;
  countModeMinutesRemaining = 0;
  countModeWhiteLedCount = 0;
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
      lastRainbowUpdate = 0;
      countModeSelectedMinutes = 0;
      countModeMinutesRemaining = 0;
      countModeWhiteLedCount = 0;
      countModeState = COUNT_IDLE_STATE;
      smoothedNightLightWhite = 5.0;
      smoothedNightLightLitCount = LED_COUNT;

      if (currentMode == COUNT_MODE) {
        showCountModeIdle();
        strip.show();
      } else if (currentMode == SPIRIT_LEVEL_MODE) {
        showSpiritLevel(0.0);
        strip.show();
      } else if (currentMode == OFF_MODE) {
        strip.clear();
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
      countModeSelectedMinutes = 0;
      countModeMinutesRemaining = 0;
      countModeWhiteLedCount = 0;
      Serial.println("State -> OFF");
    }
  }
}

void updateDefaultMode() {
  applyModeBrightness();
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
  applyModeBrightness();
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
  applyModeBrightness();
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
  applyModeBrightness();
  unsigned long now = millis();

  if (countModeState == COUNT_IDLE_STATE) {
    showCountModeIdle();
    strip.show();
    return;
  }

  if (countModeState == COUNT_SELECTING_STATE) {
    if (countHoldActive && now - lastCountModeUpdate >= COUNT_UP_INTERVAL_MS) {
      unsigned long elapsed = now - lastCountModeUpdate;
      int selectionSteps = static_cast<int>(elapsed / COUNT_UP_INTERVAL_MS);
      lastCountModeUpdate += static_cast<unsigned long>(selectionSteps) * COUNT_UP_INTERVAL_MS;
      countModeSelectedMinutes = min(LED_COUNT, countModeSelectedMinutes + selectionSteps);
    }

    renderCountMode(0, countModeSelectedMinutes);
    strip.show();
    return;
  }

  if (countModeState == COUNT_RUNNING_STATE) {
    while (countModeState == COUNT_RUNNING_STATE && now - lastCountModeUpdate >= COUNT_DOWN_INTERVAL_MS) {
      lastCountModeUpdate += COUNT_DOWN_INTERVAL_MS;
      if (countModeWhiteLedCount > 0) {
        countModeWhiteLedCount--;
      }

      if (countModeWhiteLedCount <= 0) {
        countModeMinutesRemaining--;

        if (countModeMinutesRemaining <= 0) {
          countModeState = COUNT_IDLE_STATE;
          countModeSelectedMinutes = 0;
          countModeMinutesRemaining = 0;
          countModeWhiteLedCount = 0;
          playCountdownFinishedSound();
          showCountModeIdle();
          strip.show();
          return;
        }

        countModeWhiteLedCount = LED_COUNT;
      }
    }

    renderCountMode(countModeMinutesRemaining, countModeWhiteLedCount);
    strip.show();
  }
}

void updateRainbowMode() {
  applyModeBrightness();
  unsigned long now = millis();
  if (now - lastRainbowUpdate < RAINBOW_UPDATE_MS) {
    return;
  }

  float saberRight = 0.0;
  float saberTip = 0.0;
  float saberUp = 0.0;
  readOrientation(saberRight, saberTip, saberUp);

  sensors_event_t accelEvent;
  sensors_event_t gyroEvent;
  sensors_event_t tempEvent;
  mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);
  float movement = fabs(gyroEvent.gyro.x) + fabs(gyroEvent.gyro.y) + fabs(gyroEvent.gyro.z);

  if (movement > SWING_THRESHOLD && now - lastRainbowSwingTime > 400) {
    cycleRainbowPalette();
    lastRainbowSwingTime = now;
  }

  lastRainbowUpdate = now;
  rainbowOffset++;
  rainbowTemperature = clamp01((saberRight + GRAVITY_REFERENCE) / (2.0 * GRAVITY_REFERENCE));

  int whiteOffset = static_cast<int>(rainbowTemperature * 70.0);
  int pulseWhite = static_cast<int>(20.0 + 35.0 * (0.5 + 0.5 * sin(now / 180.0)));

  for (int i = 0; i < LED_COUNT; i++) {
    byte wheelPos = static_cast<byte>((i * 256 / LED_COUNT + rainbowOffset) & 0xFF);
    uint32_t colorValue = colorWheel(getRainbowPaletteWheelPos(wheelPos), whiteOffset + pulseWhite);
    strip.setPixelColor(i, colorValue);
  }

  strip.show();
}

void updateSpiritLevelMode() {
  applyModeBrightness();

  float saberRight = 0.0;
  float saberTip = 0.0;
  float saberUp = 0.0;
  readOrientation(saberRight, saberTip, saberUp);

  float spiritLevelValue = saberRight;
  if (spiritLevelAxisMode == SPIRIT_LEVEL_AXIS_TIP) {
    spiritLevelValue = saberTip;
  } else if (spiritLevelAxisMode == SPIRIT_LEVEL_AXIS_UP) {
    spiritLevelValue = saberUp;
  }

  showSpiritLevel(spiritLevelValue);
  strip.show();
}

void cycleSpiritLevelAxis() {
  spiritLevelAxisMode = static_cast<SpiritLevelAxisMode>((spiritLevelAxisMode + 1) % 3);

  if (spiritLevelAxisMode == SPIRIT_LEVEL_AXIS_RIGHT) {
    Serial.println("Spirit level axis -> right");
  } else if (spiritLevelAxisMode == SPIRIT_LEVEL_AXIS_TIP) {
    Serial.println("Spirit level axis -> tip");
  } else {
    Serial.println("Spirit level axis -> up");
  }
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

void renderCountMode(int greenLedCount, int whiteLedCount) {
  showCountModeIdle();

  for (int i = 0; i < greenLedCount && i < LED_COUNT; i++) {
    int redValue = 0;
    int greenValue = 0;
    int blueValue = 0;
    int whiteValue = 0;

    getCountModeBaseColor(i, redValue, greenValue, blueValue, whiteValue);
    greenValue = min(255, greenValue + 90);
    strip.setPixelColor(i, strip.Color(redValue, greenValue, blueValue, whiteValue));
  }

  for (int i = 0; i < whiteLedCount && i < LED_COUNT; i++) {
    int redValue = 0;
    int greenValue = 0;
    int blueValue = 0;
    int whiteValue = 0;

    getCountModeBaseColor(i, redValue, greenValue, blueValue, whiteValue);
    if (i < greenLedCount) {
      greenValue = min(255, greenValue + 90);
    }
    whiteValue = min(255, whiteValue + 45);
    strip.setPixelColor(i, strip.Color(redValue, greenValue, blueValue, whiteValue));
  }
}

void playCountdownFinishedSound() {
  tone(BUZZER_PIN, 740, 90);
  delay(120);
  tone(BUZZER_PIN, 660, 90);
  delay(120);
  tone(BUZZER_PIN, 740, 120);
  delay(150);
}

uint32_t colorWheel(byte wheelPos, int whiteOffset) {
  byte scaledPos = 255 - wheelPos;
  int redValue = 0;
  int greenValue = 0;
  int blueValue = 0;

  if (scaledPos < 85) {
    redValue = 255 - scaledPos * 3;
    greenValue = 0;
    blueValue = scaledPos * 3;
  } else if (scaledPos < 170) {
    scaledPos -= 85;
    redValue = 0;
    greenValue = scaledPos * 3;
    blueValue = 255 - scaledPos * 3;
  } else {
    scaledPos -= 170;
    redValue = scaledPos * 3;
    greenValue = 255 - scaledPos * 3;
    blueValue = 0;
  }

  int whiteValue = constrain(whiteOffset, 0, 180);
  return strip.Color(redValue, greenValue, blueValue, whiteValue);
}

void showSpiritLevel(float saberRight) {
  strip.clear();

  float normalizedTilt = clamp01(fabs(saberRight) / (GRAVITY_REFERENCE * 0.7));
  int leftOuter = 14 - static_cast<int>(14.0 * normalizedTilt + 0.5);
  int leftInner = 14 + static_cast<int>(15.0 * normalizedTilt + 0.5);
  int rightInner = 44 - static_cast<int>(14.0 * normalizedTilt + 0.5);
  int rightOuter = 44 + static_cast<int>(15.0 * normalizedTilt + 0.5);

  strip.setPixelColor(leftOuter, strip.Color(0, 120, 0, 0));
  strip.setPixelColor(leftInner, strip.Color(0, 120, 0, 0));
  strip.setPixelColor(rightInner, strip.Color(0, 120, 0, 0));
  strip.setPixelColor(rightOuter, strip.Color(0, 120, 0, 0));
}

void cycleRainbowPalette() {
  rainbowPaletteMode = static_cast<RainbowPaletteMode>((rainbowPaletteMode + 1) % 4);

  if (rainbowPaletteMode == RAINBOW_ALL_COLORS) {
    Serial.println("Rainbow palette -> all colors");
  } else if (rainbowPaletteMode == RAINBOW_REDDISH) {
    Serial.println("Rainbow palette -> reddish");
  } else if (rainbowPaletteMode == RAINBOW_BLUISH) {
    Serial.println("Rainbow palette -> blueish");
  } else {
    Serial.println("Rainbow palette -> greenish");
  }
}

byte getRainbowPaletteWheelPos(byte wheelPos) {
  if (rainbowPaletteMode == RAINBOW_ALL_COLORS) {
    return wheelPos;
  }

  if (rainbowPaletteMode == RAINBOW_REDDISH) {
    return static_cast<byte>((wheelPos / 3) + 170);
  }

  if (rainbowPaletteMode == RAINBOW_BLUISH) {
    return static_cast<byte>((wheelPos / 3) + 85);
  }

  return static_cast<byte>(wheelPos / 3);
}

void getCountModeBaseColor(int pixelIndex, int& redValue, int& greenValue, int& blueValue, int& whiteValue) {
  redValue = 0;
  greenValue = 0;
  blueValue = 0;
  whiteValue = 0;

  if ((pixelIndex + 1) % 10 == 5) {
    redValue = 5;
    return;
  }

  if ((pixelIndex + 1) % 10 == 0) {
    redValue = 15;
    greenValue = 2;
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
