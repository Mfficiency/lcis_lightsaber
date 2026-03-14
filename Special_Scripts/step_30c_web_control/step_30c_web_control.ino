// SPECIAL: Step 30C Web Control
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
// Web controls:
// - connect to the "Lightsaber-Control" Wi-Fi network
// - open 192.168.4.1 in a browser
// - choose power, mode, master brightness, and master hue
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
#include <DNSServer.h>
#include <WiFi.h>
#include <WebServer.h>

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
const byte DNS_PORT = 53;
const char* ACCESS_POINT_SSID = "Lightsaber-Control";
const char* ACCESS_POINT_PASSWORD = "lightsaber";

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
  COUNT_RUNNING_STATE,
  COUNT_PAUSED_STATE
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
int masterBrightnessPercent = 100;
int masterHueDegrees = 0;

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
DNSServer dnsServer;
WebServer server(80);

void handleButton();
void processPendingShortPress();
void handleShortPress();
void handleDoublePress();
void cycleMode();
void setMode(SaberMode newMode, bool playSoundEnabled);
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
void renderCountMode(int whiteLedCount, int minuteIndicatorCount);
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
void setupWebServer();
void handleWebRoot();
void handleWebControl();
void handleWebStatus();
void handleCaptivePortalRedirect();
void refreshModeOutput();
void startTimerFromWeb();
void stopTimerFromWeb();
void resetTimerFromWeb();
int getModeBaseBrightness();
uint32_t makeBladeColor(int red, int green, int blue, int white);
void applyMasterHueShift(int& red, int& green, int& blue);
void hueToRgb(float hueDegrees, int& red, int& green, int& blue);
const char* getModeName(SaberMode mode);
const char* getStateName(SaberState state);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  Wire.begin(GYRO_SDA_PIN, GYRO_SCL_PIN);
  Serial.println("Step 30c web control ready. State -> OFF");
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

  setupWebServer();
  showCurrentMode();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
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
      bool resetActiveCountdown =
        (countModeState == COUNT_RUNNING_STATE || countModeState == COUNT_PAUSED_STATE);
      pendingShortPress = false;
      countHoldActive = true;
      if (resetActiveCountdown) {
        countModeSelectedMinutes = 0;
        countModeMinutesRemaining = 0;
        countModeWhiteLedCount = 0;
      }
      countModeState = COUNT_SELECTING_STATE;
      if (!resetActiveCountdown && countModeSelectedMinutes <= 0) {
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
  setMode(static_cast<SaberMode>((currentMode + 1) % 7), true);
}

void setMode(SaberMode newMode, bool playSoundEnabled) {
  currentMode = newMode;

  if (playSoundEnabled) {
    playModeCycleSound();
  }

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
  refreshModeOutput();
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
  int adjustedBrightness = (getModeBaseBrightness() * masterBrightnessPercent) / 100;
  strip.setBrightness(constrain(adjustedBrightness, 0, 255));
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
    strip.setPixelColor(leftIndex, makeBladeColor(bladeRed, bladeGreen, bladeBlue, bladeWhite));

    if (rightIndex != leftIndex) {
      strip.setPixelColor(rightIndex, makeBladeColor(bladeRed, bladeGreen, bladeBlue, bladeWhite));
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
      lastCountModeUpdate = now;

      if (countModeSelectedMinutes < LED_COUNT) {
        countModeSelectedMinutes++;
      }
    }

    renderCountMode(countModeSelectedMinutes, min(2, countModeSelectedMinutes));
    strip.show();
    return;
  }

  if (countModeState == COUNT_PAUSED_STATE) {
    renderCountMode(countModeWhiteLedCount, min(2, countModeMinutesRemaining));
    strip.show();
    return;
  }

  if (countModeState == COUNT_RUNNING_STATE) {
    if (now - lastCountModeUpdate >= COUNT_DOWN_INTERVAL_MS) {
      lastCountModeUpdate = now;

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

    renderCountMode(countModeWhiteLedCount, min(2, countModeMinutesRemaining));
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
    strip.setPixelColor(i, makeBladeColor(red, green, blue, white));
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
    strip.setPixelColor(sparkIndex, makeBladeColor(redValue, 30, 0, whiteValue));
  }

  strip.show();
}

void showNightLightBlade(int whiteValue, int litCount) {
  strip.clear();

  int startIndex = (LED_COUNT - litCount) / 2;
  int endIndex = startIndex + litCount - 1;

  for (int i = startIndex; i <= endIndex; i++) {
    strip.setPixelColor(i, makeBladeColor(0, 0, 0, whiteValue));
  }
}

void showCountModeIdle() {
  strip.clear();

  for (int i = 4; i < LED_COUNT; i += 10) {
    strip.setPixelColor(i, makeBladeColor(5, 0, 0, 0));
  }

  for (int i = 9; i < LED_COUNT; i += 10) {
    strip.setPixelColor(i, makeBladeColor(15, 2, 0, 0));
  }
}

void renderCountMode(int whiteLedCount, int minuteIndicatorCount) {
  showCountModeIdle();

  for (int i = 0; i < whiteLedCount && i < LED_COUNT; i++) {
    int redValue = 0;
    int greenValue = 0;
    int blueValue = 0;
    int whiteValue = 0;

    getCountModeBaseColor(i, redValue, greenValue, blueValue, whiteValue);
    whiteValue = min(255, whiteValue + 5);
    strip.setPixelColor(i, makeBladeColor(redValue, greenValue, blueValue, whiteValue));
  }

  if (minuteIndicatorCount >= 1) {
    int redValue = 0;
    int greenValue = 0;
    int blueValue = 0;
    int whiteValue = 0;
    getCountModeBaseColor(0, redValue, greenValue, blueValue, whiteValue);
    whiteValue = min(255, whiteValue + 40);
    strip.setPixelColor(0, makeBladeColor(redValue, greenValue, blueValue, whiteValue));
  }

  if (minuteIndicatorCount >= 2) {
    int redValue = 0;
    int greenValue = 0;
    int blueValue = 0;
    int whiteValue = 0;
    getCountModeBaseColor(1, redValue, greenValue, blueValue, whiteValue);
    whiteValue = min(255, whiteValue + 40);
    strip.setPixelColor(1, makeBladeColor(redValue, greenValue, blueValue, whiteValue));
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
  return makeBladeColor(redValue, greenValue, blueValue, whiteValue);
}

void showSpiritLevel(float saberRight) {
  strip.clear();

  float normalizedTilt = clamp01(fabs(saberRight) / (GRAVITY_REFERENCE * 0.7));
  int leftOuter = 14 - static_cast<int>(14.0 * normalizedTilt + 0.5);
  int leftInner = 14 + static_cast<int>(15.0 * normalizedTilt + 0.5);
  int rightInner = 44 - static_cast<int>(14.0 * normalizedTilt + 0.5);
  int rightOuter = 44 + static_cast<int>(15.0 * normalizedTilt + 0.5);

  strip.setPixelColor(leftOuter, makeBladeColor(0, 120, 0, 0));
  strip.setPixelColor(leftInner, makeBladeColor(0, 120, 0, 0));
  strip.setPixelColor(rightInner, makeBladeColor(0, 120, 0, 0));
  strip.setPixelColor(rightOuter, makeBladeColor(0, 120, 0, 0));
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

void setupWebServer() {
  WiFi.mode(WIFI_AP);
  bool apStarted = WiFi.softAP(ACCESS_POINT_SSID, ACCESS_POINT_PASSWORD);

  if (apStarted) {
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    Serial.print("Web control AP ready: ");
    Serial.println(ACCESS_POINT_SSID);
    Serial.print("Open http://");
    Serial.println(WiFi.softAPIP());
    Serial.println("Captive portal redirect enabled.");
  } else {
    Serial.println("WARNING: failed to start Wi-Fi access point.");
  }

  server.on("/", HTTP_GET, handleWebRoot);
  server.on("/control", HTTP_GET, handleWebControl);
  server.on("/status", HTTP_GET, handleWebStatus);
  server.on("/generate_204", HTTP_GET, handleCaptivePortalRedirect);
  server.on("/gen_204", HTTP_GET, handleCaptivePortalRedirect);
  server.on("/hotspot-detect.html", HTTP_GET, handleCaptivePortalRedirect);
  server.on("/canonical.html", HTTP_GET, handleCaptivePortalRedirect);
  server.on("/ncsi.txt", HTTP_GET, handleCaptivePortalRedirect);
  server.on("/connecttest.txt", HTTP_GET, handleCaptivePortalRedirect);
  server.on("/fwlink", HTTP_GET, handleCaptivePortalRedirect);
  server.on("/redirect", HTTP_GET, handleCaptivePortalRedirect);
  server.onNotFound(handleCaptivePortalRedirect);
  server.begin();
}

void handleWebRoot() {
  String html = R"HTML(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Lightsaber Control</title>
  <style>
    :root { color-scheme: dark; }
    body { font-family: Arial, sans-serif; background: #101820; color: #f2f5f7; margin: 0; padding: 20px; }
    .panel { max-width: 720px; margin: 0 auto; background: #182430; border-radius: 16px; padding: 20px; box-shadow: 0 12px 40px rgba(0,0,0,.28); }
    h1 { margin-top: 0; }
    .buttons { display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 10px; margin: 16px 0; }
    button { border: 0; border-radius: 999px; padding: 12px 14px; font-size: 15px; cursor: pointer; background: #2a82da; color: white; }
    button.alt { background: #384656; }
    button.small { padding: 8px 12px; font-size: 13px; }
    button:disabled { background: #5a6470; color: #ced5db; cursor: not-allowed; opacity: .6; }
    label { display: block; margin-top: 18px; font-weight: bold; }
    input[type=range] { width: 100%; }
    input:disabled { opacity: .45; }
    .status { margin-top: 14px; padding: 12px; background: #0d141b; border-radius: 12px; }
    .value { font-weight: normal; color: #8fd3ff; }
    .small { color: #9fb2c3; font-size: 14px; }
    .hidden { display: none; }
    .topbar { display: flex; align-items: center; justify-content: space-between; gap: 12px; margin-bottom: 10px; }
    .section-hidden { display: none; }
  </style>
</head>
<body>
  <div class="panel">
    <div id="mainView">
      <h1>Lightsaber Control</h1>
      <div class="small">Connect to the board Wi-Fi and use this page to control power and modes.</div>
      <div class="buttons">
        <button id="powerOnButton" onclick="sendControl('state=on')">Power On</button>
        <button id="powerOffButton" class="alt" onclick="sendControl('state=off')">Power Off</button>
      </div>
      <div class="buttons">
        <button class="mode-button" onclick="sendControl('mode=0')">Default</button>
        <button class="mode-button" onclick="sendControl('mode=2')">Nightlight</button>
        <button class="mode-button" onclick="sendControl('mode=3')">Timer</button>
        <button class="mode-button" onclick="sendControl('mode=4')">Rainbow</button>
        <button class="mode-button" onclick="sendControl('mode=5')">Level</button>
        <button id="settingsButton" class="alt" onclick="showSettings()">Settings</button>
      </div>
      <div id="timerSection" class="section-hidden">
        <label for="timerMinutes">Timer Minutes <span id="timerMinutesValue" class="value"></span></label>
        <input id="timerMinutes" type="number" min="1" max="60" value="5" oninput="document.getElementById('timerMinutesValue').textContent=this.value + ' min'">
      <div class="buttons">
        <button class="timer-button" onclick="sendTimerAction('start')">Start</button>
        <button class="timer-button alt" onclick="sendTimerAction('stop')">Stop</button>
        <button class="timer-button alt" onclick="sendTimerAction('reset')">Reset</button>
      </div>
      <div class="small" id="timerCountdown">Timer not running.</div>
      </div>
      <div class="status" id="status">Loading status...</div>
    </div>
    <div id="settingsView" class="hidden">
      <div class="topbar">
        <button class="alt" onclick="showMain()">Back</button>
        <button class="small alt" onclick="showAbout()">About</button>
      </div>
      <h1>Settings</h1>
      <label for="brightness">Master Brightness <span id="brightnessValue" class="value"></span></label>
      <input id="brightness" type="range" min="0" max="100" value="100" oninput="document.getElementById('brightnessValue').textContent=this.value + '%'">
      <button id="applyBrightnessButton" class="alt" onclick="sendSlider('brightness', document.getElementById('brightness').value)">Apply Brightness</button>
      <label for="hue">Master Hue <span id="hueValue" class="value"></span></label>
      <input id="hue" type="range" min="0" max="359" value="0" oninput="document.getElementById('hueValue').textContent=this.value + ' deg'">
      <button id="applyHueButton" class="alt" onclick="sendSlider('hue', document.getElementById('hue').value)">Apply Hue</button>
      <div class="status" id="settingsStatus">Loading status...</div>
    </div>
  </div>
  <script>
    let brightnessDirty = false;
    let hueDirty = false;
    let localTimerSeconds = null;
    let countdownTickHandle = null;
    let countdownSyncHandle = null;

    function showMain() {
      document.getElementById('mainView').classList.remove('hidden');
      document.getElementById('settingsView').classList.add('hidden');
    }
    function showSettings() {
      document.getElementById('mainView').classList.add('hidden');
      document.getElementById('settingsView').classList.remove('hidden');
    }
    function showAbout() {
      window.open('https://mfficiency.com', '_blank', 'noopener');
    }
    function setDisabledForSelector(selector, disabled) {
      document.querySelectorAll(selector).forEach((element) => {
        element.disabled = disabled;
      });
    }
    function formatTimerCountdown(totalSeconds) {
      const safeSeconds = Math.max(0, totalSeconds);
      const minutes = Math.floor(safeSeconds / 60);
      const seconds = safeSeconds % 60;
      return minutes + ':' + String(seconds).padStart(2, '0');
    }
    function stopLocalCountdown() {
      if (countdownTickHandle) {
        clearInterval(countdownTickHandle);
        countdownTickHandle = null;
      }
      if (countdownSyncHandle) {
        clearTimeout(countdownSyncHandle);
        countdownSyncHandle = null;
      }
    }
    function renderLocalCountdown(timerState) {
      const countdownElement = document.getElementById('timerCountdown');
      if (timerState === 'Running' && localTimerSeconds !== null) {
        countdownElement.textContent = 'Time left: ' + formatTimerCountdown(localTimerSeconds);
        return;
      }
      if (timerState === 'Paused' && localTimerSeconds !== null) {
        countdownElement.textContent = 'Paused at ' + formatTimerCountdown(localTimerSeconds);
        return;
      }
      countdownElement.textContent = 'Timer not running.';
    }
    function scheduleCountdownSync() {
      if (countdownSyncHandle) {
        clearTimeout(countdownSyncHandle);
      }
      countdownSyncHandle = setTimeout(() => {
        refreshStatus();
      }, 60000);
    }
    function startLocalCountdown(secondsRemaining) {
      stopLocalCountdown();
      localTimerSeconds = Math.max(0, secondsRemaining);
      renderLocalCountdown('Running');
      countdownTickHandle = setInterval(() => {
        if (localTimerSeconds === null || localTimerSeconds <= 0) {
          stopLocalCountdown();
          localTimerSeconds = 0;
          renderLocalCountdown('Idle');
          return;
        }
        localTimerSeconds -= 1;
        renderLocalCountdown('Running');
      }, 1000);
      scheduleCountdownSync();
    }
    function syncTimerState(status) {
      if (status.timerState === 'Running') {
        startLocalCountdown(status.timerSecondsRemaining);
        return;
      }
      stopLocalCountdown();
      if (status.timerState === 'Paused') {
        localTimerSeconds = status.timerSecondsRemaining;
        renderLocalCountdown('Paused');
        return;
      }
      localTimerSeconds = null;
      renderLocalCountdown('Idle');
    }
    function applyPowerState(status) {
      const isOn = status.state === 'On';
      const showTimerControls = isOn && status.mode === 'Count';
      document.getElementById('powerOnButton').disabled = isOn;
      document.getElementById('powerOffButton').disabled = !isOn;
      document.getElementById('settingsButton').disabled = !isOn;
      document.getElementById('timerMinutes').disabled = !isOn;
      document.getElementById('brightness').disabled = !isOn;
      document.getElementById('hue').disabled = !isOn;
      document.getElementById('applyBrightnessButton').disabled = !isOn;
      document.getElementById('applyHueButton').disabled = !isOn;
      setDisabledForSelector('.mode-button', !isOn);
      setDisabledForSelector('.timer-button', !isOn);
      document.getElementById('timerSection').classList.toggle('section-hidden', !showTimerControls);
    }
    async function sendControl(query) {
      await fetch('/control?' + query);
      await refreshStatus();
    }
    async function sendSlider(name, value) {
      await fetch('/control?' + name + '=' + encodeURIComponent(value));
      if (name === 'brightness') {
        brightnessDirty = false;
      } else if (name === 'hue') {
        hueDirty = false;
      }
      await refreshStatus();
    }
    async function sendTimerAction(action) {
      const minutes = document.getElementById('timerMinutes').value;
      await fetch('/control?timerAction=' + encodeURIComponent(action) + '&timerMinutes=' + encodeURIComponent(minutes));
      await refreshStatus();
    }
    async function refreshStatus() {
      const response = await fetch('/status');
      const status = await response.json();
      applyPowerState(status);
      syncTimerState(status);
      document.getElementById('timerMinutes').value = status.timerSelectedMinutes;
      document.getElementById('timerMinutesValue').textContent = status.timerSelectedMinutes + ' min';
      if (!brightnessDirty) {
        document.getElementById('brightness').value = status.masterBrightness;
        document.getElementById('brightnessValue').textContent = status.masterBrightness + '%';
      }
      if (!hueDirty) {
        document.getElementById('hue').value = status.masterHue;
        document.getElementById('hueValue').textContent = status.masterHue + ' deg';
      }
      const statusHtml =
        'State: <span class="value">' + status.state + '</span><br>' +
        'Mode: <span class="value">' + status.mode + '</span><br>' +
        'Brightness: <span class="value">' + status.masterBrightness + '%</span><br>' +
        'Hue: <span class="value">' + status.masterHue + ' deg</span><br>' +
        'Timer state: <span class="value">' + status.timerState + '</span><br>' +
        'Timer minutes: <span class="value">' + status.timerSelectedMinutes + '</span><br>' +
        'Minutes remaining: <span class="value">' + status.timerMinutesRemaining + '</span>';
      document.getElementById('status').innerHTML = statusHtml;
      document.getElementById('settingsStatus').innerHTML = statusHtml;
    }
    document.getElementById('brightness').addEventListener('input', () => {
      brightnessDirty = true;
    });
    document.getElementById('brightness').addEventListener('change', () => {
      brightnessDirty = true;
    });
    document.getElementById('hue').addEventListener('input', () => {
      hueDirty = true;
    });
    document.getElementById('hue').addEventListener('change', () => {
      hueDirty = true;
    });
    refreshStatus();
    setInterval(() => {
      if (countdownTickHandle === null) {
        refreshStatus();
      }
    }, 5000);
  </script>
</body>
</html>
)HTML";

  server.send(200, "text/html", html);
}

void handleWebControl() {
  if (server.hasArg("timerMinutes")) {
    countModeSelectedMinutes = constrain(server.arg("timerMinutes").toInt(), 1, LED_COUNT);
    if (currentMode == COUNT_MODE && countModeState == COUNT_IDLE_STATE) {
      refreshModeOutput();
    }
  }

  if (server.hasArg("state")) {
    String stateValue = server.arg("state");

    if (stateValue == "on" && currentState == OFF_STATE) {
      startTurnOn();
    } else if (stateValue == "off" && currentState == ON_STATE) {
      startTurnOff();
    }
  }

  if (server.hasArg("mode")) {
    int modeValue = constrain(server.arg("mode").toInt(), 0, 6);
    setMode(static_cast<SaberMode>(modeValue), false);
  }

  if (server.hasArg("timerAction")) {
    String timerAction = server.arg("timerAction");

    if (timerAction == "start") {
      startTimerFromWeb();
    } else if (timerAction == "stop") {
      stopTimerFromWeb();
    } else if (timerAction == "reset") {
      resetTimerFromWeb();
    }
  }

  if (server.hasArg("brightness")) {
    masterBrightnessPercent = constrain(server.arg("brightness").toInt(), 0, 100);
    applyModeBrightness();
    refreshModeOutput();
  }

  if (server.hasArg("hue")) {
    masterHueDegrees = constrain(server.arg("hue").toInt(), 0, 359);
    refreshModeOutput();
  }

  server.send(200, "text/plain", "ok");
}

void handleWebStatus() {
  String json = "{";
  json += "\"state\":\"";
  json += getStateName(currentState);
  json += "\",\"mode\":\"";
  json += getModeName(currentMode);
  json += "\",\"masterBrightness\":";
  json += masterBrightnessPercent;
  json += ",\"masterHue\":";
  json += masterHueDegrees;
  json += ",\"timerState\":\"";
  if (countModeState == COUNT_IDLE_STATE) {
    json += "Idle";
  } else if (countModeState == COUNT_SELECTING_STATE) {
    json += "Selecting";
  } else if (countModeState == COUNT_RUNNING_STATE) {
    json += "Running";
  } else {
    json += "Paused";
  }
  json += "\",\"timerSelectedMinutes\":";
  json += max(1, countModeSelectedMinutes);
  json += ",\"timerMinutesRemaining\":";
  json += countModeMinutesRemaining;
  json += ",\"timerSecondsRemaining\":";
  if (countModeState == COUNT_RUNNING_STATE || countModeState == COUNT_PAUSED_STATE) {
    json += max(0, ((countModeMinutesRemaining - 1) * LED_COUNT) + countModeWhiteLedCount);
  } else if (countModeSelectedMinutes > 0) {
    json += countModeSelectedMinutes * LED_COUNT;
  } else {
    json += 0;
  }
  json += "}";

  server.send(200, "application/json", json);
}

void handleCaptivePortalRedirect() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.sendHeader("Location", "http://192.168.4.1/");
  server.send(302, "text/plain", "Redirecting to Lightsaber Control");
}

void startTimerFromWeb() {
  if (currentMode != COUNT_MODE) {
    setMode(COUNT_MODE, false);
  }

  if (countModeSelectedMinutes <= 0) {
    countModeSelectedMinutes = 1;
  }

  if (countModeState == COUNT_PAUSED_STATE && countModeMinutesRemaining > 0 && countModeWhiteLedCount > 0) {
    countModeState = COUNT_RUNNING_STATE;
    lastCountModeUpdate = millis();
    Serial.println("Timer -> resumed from web");
    return;
  }

  countModeMinutesRemaining = countModeSelectedMinutes;
  countModeWhiteLedCount = LED_COUNT;
  countModeState = COUNT_RUNNING_STATE;
  lastCountModeUpdate = millis();
  Serial.println("Timer -> started from web");
}

void stopTimerFromWeb() {
  if (currentMode == COUNT_MODE && countModeState == COUNT_RUNNING_STATE) {
    countModeState = COUNT_PAUSED_STATE;
    Serial.println("Timer -> paused from web");
  }
}

void resetTimerFromWeb() {
  countModeState = COUNT_IDLE_STATE;
  countModeMinutesRemaining = 0;
  countModeWhiteLedCount = 0;
  countHoldActive = false;
  Serial.println("Timer -> reset from web");

  if (currentMode == COUNT_MODE) {
    refreshModeOutput();
  }
}

void refreshModeOutput() {
  if (currentState != ON_STATE) {
    return;
  }

  if (currentMode == DEFAULT_MODE || currentMode == COLOR_CHOOSE_MODE) {
    fillCurrentBladeColor();
    strip.show();
    return;
  }

  if (currentMode == COUNT_MODE) {
    if (countModeState == COUNT_RUNNING_STATE || countModeState == COUNT_PAUSED_STATE) {
      renderCountMode(countModeWhiteLedCount, min(2, countModeMinutesRemaining));
    } else if (countModeSelectedMinutes > 0) {
      renderCountMode(countModeSelectedMinutes, min(2, countModeSelectedMinutes));
    } else {
      showCountModeIdle();
    }
    strip.show();
    return;
  }

  if (currentMode == OFF_MODE) {
    strip.clear();
    strip.show();
  }
}

int getModeBaseBrightness() {
  if (currentMode == DEFAULT_MODE) {
    return SABER_BRIGHTNESS;
  }

  if (currentMode == COLOR_CHOOSE_MODE) {
    return COLOR_CHOOSE_BRIGHTNESS;
  }

  if (currentMode == NIGHTLIGHT_MODE) {
    return NIGHTLIGHT_BRIGHTNESS;
  }

  if (currentMode == COUNT_MODE) {
    return COUNT_MODE_BRIGHTNESS;
  }

  if (currentMode == RAINBOW_MODE) {
    return RAINBOW_BRIGHTNESS;
  }

  if (currentMode == SPIRIT_LEVEL_MODE) {
    return SPIRIT_LEVEL_BRIGHTNESS;
  }

  return OFF_MODE_BRIGHTNESS;
}

uint32_t makeBladeColor(int red, int green, int blue, int white) {
  int shiftedRed = constrain(red, 0, 255);
  int shiftedGreen = constrain(green, 0, 255);
  int shiftedBlue = constrain(blue, 0, 255);
  int shiftedWhite = constrain(white, 0, 255);

  applyMasterHueShift(shiftedRed, shiftedGreen, shiftedBlue);

  if (shiftedRed == 0 && shiftedGreen == 0 && shiftedBlue == 0 && shiftedWhite > 0 && masterHueDegrees != 0) {
    int tintRed = 0;
    int tintGreen = 0;
    int tintBlue = 0;
    hueToRgb(static_cast<float>(masterHueDegrees), tintRed, tintGreen, tintBlue);
    shiftedRed = (tintRed * shiftedWhite) / 765;
    shiftedGreen = (tintGreen * shiftedWhite) / 765;
    shiftedBlue = (tintBlue * shiftedWhite) / 765;
  }

  return strip.Color(shiftedRed, shiftedGreen, shiftedBlue, shiftedWhite);
}

void applyMasterHueShift(int& red, int& green, int& blue) {
  if (masterHueDegrees == 0) {
    return;
  }

  int maxChannel = max(red, max(green, blue));
  int minChannel = min(red, min(green, blue));
  int delta = maxChannel - minChannel;

  if (delta == 0 || maxChannel == 0) {
    return;
  }

  float hue = 0.0;
  float saturation = static_cast<float>(delta) / static_cast<float>(maxChannel);
  float value = static_cast<float>(maxChannel) / 255.0;

  if (maxChannel == red) {
    hue = 60.0 * fmod(((green - blue) / static_cast<float>(delta)), 6.0f);
  } else if (maxChannel == green) {
    hue = 60.0 * (((blue - red) / static_cast<float>(delta)) + 2.0f);
  } else {
    hue = 60.0 * (((red - green) / static_cast<float>(delta)) + 4.0f);
  }

  if (hue < 0.0) {
    hue += 360.0;
  }

  hue += static_cast<float>(masterHueDegrees);
  while (hue >= 360.0) {
    hue -= 360.0;
  }

  float chroma = value * saturation;
  float hueSection = hue / 60.0;
  float x = chroma * (1.0 - fabs(fmod(hueSection, 2.0f) - 1.0f));
  float match = value - chroma;
  float redPrime = 0.0;
  float greenPrime = 0.0;
  float bluePrime = 0.0;

  if (hueSection < 1.0) {
    redPrime = chroma;
    greenPrime = x;
  } else if (hueSection < 2.0) {
    redPrime = x;
    greenPrime = chroma;
  } else if (hueSection < 3.0) {
    greenPrime = chroma;
    bluePrime = x;
  } else if (hueSection < 4.0) {
    greenPrime = x;
    bluePrime = chroma;
  } else if (hueSection < 5.0) {
    redPrime = x;
    bluePrime = chroma;
  } else {
    redPrime = chroma;
    bluePrime = x;
  }

  red = constrain(static_cast<int>((redPrime + match) * 255.0f + 0.5f), 0, 255);
  green = constrain(static_cast<int>((greenPrime + match) * 255.0f + 0.5f), 0, 255);
  blue = constrain(static_cast<int>((bluePrime + match) * 255.0f + 0.5f), 0, 255);
}

void hueToRgb(float hueDegrees, int& red, int& green, int& blue) {
  float chroma = 1.0;
  float hueSection = hueDegrees / 60.0;
  float x = chroma * (1.0 - fabs(fmod(hueSection, 2.0f) - 1.0f));
  float redPrime = 0.0;
  float greenPrime = 0.0;
  float bluePrime = 0.0;

  if (hueSection < 1.0) {
    redPrime = chroma;
    greenPrime = x;
  } else if (hueSection < 2.0) {
    redPrime = x;
    greenPrime = chroma;
  } else if (hueSection < 3.0) {
    greenPrime = chroma;
    bluePrime = x;
  } else if (hueSection < 4.0) {
    greenPrime = x;
    bluePrime = chroma;
  } else if (hueSection < 5.0) {
    redPrime = x;
    bluePrime = chroma;
  } else {
    redPrime = chroma;
    bluePrime = x;
  }

  red = static_cast<int>(redPrime * 255.0f + 0.5f);
  green = static_cast<int>(greenPrime * 255.0f + 0.5f);
  blue = static_cast<int>(bluePrime * 255.0f + 0.5f);
}

const char* getModeName(SaberMode mode) {
  if (mode == DEFAULT_MODE) {
    return "Default";
  }

  if (mode == COLOR_CHOOSE_MODE) {
    return "Color";
  }

  if (mode == NIGHTLIGHT_MODE) {
    return "Nightlight";
  }

  if (mode == COUNT_MODE) {
    return "Count";
  }

  if (mode == RAINBOW_MODE) {
    return "Rainbow";
  }

  if (mode == SPIRIT_LEVEL_MODE) {
    return "Spirit level";
  }

  return "LED off";
}

const char* getStateName(SaberState state) {
  if (state == OFF_STATE) {
    return "Off";
  }

  if (state == TURNING_ON_STATE) {
    return "Turning on";
  }

  if (state == ON_STATE) {
    return "On";
  }

  return "Turning off";
}
