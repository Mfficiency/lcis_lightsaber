// SPECIAL: Step 30 With Long-Press Color Selection Mode
//
// Builds on step_30_final_integrated_lightsaber.
//
// CONTROLS
// Short press in normal mode:
// - OFF -> startup
// - ON -> shutdown
//
// Long press in normal mode while ON:
// - enter color selection mode
//
// In color selection mode:
// - short press cycles through preset blade colors
// - long press exits selection mode and returns to normal mode

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
const unsigned long LONG_PRESS_MS = 900;
const float SWING_THRESHOLD = 4.0;
const float CLASH_THRESHOLD = 10.0;

enum SaberState {
  OFF_STATE,
  TURNING_ON_STATE,
  ON_STATE,
  TURNING_OFF_STATE
};

struct BladeColor {
  const char* name;
  int red;
  int green;
  int blue;
  int white;
};

const BladeColor BLADE_COLORS[] = {
  {"Green", 0, 100, 0, 100},
  {"Blue", 0, 40, 120, 70},
  {"Red", 160, 0, 0, 30},
  {"Purple", 90, 0, 90, 60},
  {"Yellow", 120, 80, 0, 50},
  {"White", 40, 40, 40, 180}
};

const int COLOR_COUNT = sizeof(BLADE_COLORS) / sizeof(BLADE_COLORS[0]);

SaberState currentState = OFF_STATE;
int currentColorIndex = 0;
int animationIndex = 0;
unsigned long lastAnimationUpdate = 0;
unsigned long lastIdleUpdate = 0;

bool selectionMode = false;
int lastButtonState = HIGH;
unsigned long buttonPressStart = 0;
bool longPressHandled = false;

Adafruit_MPU6050 mpu;
Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRBW + NEO_KHZ800);

void handleButton();
void handleShortPress();
void handleLongPress();
void enterSelectionMode();
void exitSelectionMode();
void cycleSelectedColor();
void updateStateMachine();
void startTurnOn();
void startTurnOff();
void updateIdleEffect();
void showSelectionPreview();
void handleMotionEffects();
void swingEffect();
void clashEffect();
void playStartupSound();
void playShutdownSound();
void playSelectionEnterSound();
void playSelectionExitSound();
void playColorCycleSound();
void playClashScrapeSound();
void pulseBrightness();
void fillBladeColor(int red, int green, int blue, int white);
void fillCurrentBladeColor();
void showClashSparkEffect();
const BladeColor& getCurrentBladeColor();

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  Wire.begin(GYRO_SDA_PIN, GYRO_SCL_PIN);
  Serial.println("Step 30 special mode ready. State -> OFF");
  Serial.println("Long press while ON to enter color selection mode.");

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

  if (selectionMode) {
    showSelectionPreview();
    return;
  }

  if (currentState == ON_STATE) {
    updateIdleEffect();
    handleMotionEffects();
  }
}

void handleButton() {
  int buttonState = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  if (buttonState == LOW && lastButtonState == HIGH) {
    buttonPressStart = now;
    longPressHandled = false;
  }

  if (buttonState == LOW && !longPressHandled && now - buttonPressStart >= LONG_PRESS_MS) {
    handleLongPress();
    longPressHandled = true;
  }

  if (buttonState == HIGH && lastButtonState == LOW) {
    if (!longPressHandled) {
      handleShortPress();
    }
  }

  lastButtonState = buttonState;
}

void handleShortPress() {
  Serial.println("Short press");

  if (selectionMode) {
    cycleSelectedColor();
    return;
  }

  if (currentState == OFF_STATE) {
    startTurnOn();
  } else if (currentState == ON_STATE) {
    startTurnOff();
  }
}

void handleLongPress() {
  Serial.println("Long press");

  if (selectionMode) {
    exitSelectionMode();
    return;
  }

  if (currentState == ON_STATE) {
    enterSelectionMode();
  }
}

void enterSelectionMode() {
  selectionMode = true;
  Serial.print("Selection mode -> ON. Current color: ");
  Serial.println(getCurrentBladeColor().name);
  playSelectionEnterSound();
  fillCurrentBladeColor();
  strip.show();
}

void exitSelectionMode() {
  selectionMode = false;
  lastIdleUpdate = 0;
  Serial.print("Selection mode -> OFF. Selected color: ");
  Serial.println(getCurrentBladeColor().name);
  playSelectionExitSound();
}

void cycleSelectedColor() {
  currentColorIndex++;
  if (currentColorIndex >= COLOR_COUNT) {
    currentColorIndex = 0;
  }

  Serial.print("Selected color -> ");
  Serial.println(getCurrentBladeColor().name);
  playColorCycleSound();
  fillCurrentBladeColor();
  strip.show();
}

void startTurnOn() {
  currentState = TURNING_ON_STATE;
  animationIndex = 0;
  lastAnimationUpdate = 0;
  selectionMode = false;
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
  selectionMode = false;
  Serial.println("State -> TURNING_OFF");
  playShutdownSound();
  pulseBrightness();
}

void updateStateMachine() {
  unsigned long now = millis();
  const BladeColor& bladeColor = getCurrentBladeColor();

  if (currentState == TURNING_ON_STATE && now - lastAnimationUpdate >= ANIMATION_INTERVAL_MS) {
    int leftIndex = animationIndex;
    int rightIndex = LED_COUNT - 1 - animationIndex;

    lastAnimationUpdate = now;
    strip.setPixelColor(leftIndex, strip.Color(bladeColor.red, bladeColor.green, bladeColor.blue, bladeColor.white));

    if (rightIndex != leftIndex) {
      strip.setPixelColor(rightIndex, strip.Color(bladeColor.red, bladeColor.green, bladeColor.blue, bladeColor.white));
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
  if (now - lastIdleUpdate < IDLE_INTERVAL_MS) {
    return;
  }

  const BladeColor& bladeColor = getCurrentBladeColor();

  lastIdleUpdate = now;
  int whiteValue = constrain(bladeColor.white + random(0, 21), 0, 255);

  fillBladeColor(bladeColor.red, bladeColor.green, bladeColor.blue, whiteValue);
  strip.show();
}

void showSelectionPreview() {
  static unsigned long lastPreviewUpdate = 0;
  unsigned long now = millis();

  if (now - lastPreviewUpdate < 120) {
    return;
  }

  lastPreviewUpdate = now;
  fillCurrentBladeColor();

  for (int i = 0; i < LED_COUNT; i += 8) {
    strip.setPixelColor(i, strip.Color(255, 255, 255, 255));
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
  const BladeColor& bladeColor = getCurrentBladeColor();

  Serial.println("Swing detected -> blue tint");
  fillBladeColor(
    constrain(bladeColor.red / 2, 0, 255),
    constrain(bladeColor.green - 10, 0, 255),
    constrain(bladeColor.blue + 35, 0, 255),
    constrain(bladeColor.white + 10, 0, 255)
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

void playSelectionEnterSound() {
  tone(BUZZER_PIN, 700, 60);
  delay(80);
  tone(BUZZER_PIN, 980, 90);
  delay(100);
}

void playSelectionExitSound() {
  tone(BUZZER_PIN, 980, 60);
  delay(80);
  tone(BUZZER_PIN, 700, 90);
  delay(100);
}

void playColorCycleSound() {
  tone(BUZZER_PIN, 1200, 35);
  delay(45);
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
  const BladeColor& bladeColor = getCurrentBladeColor();
  fillBladeColor(bladeColor.red, bladeColor.green, bladeColor.blue, bladeColor.white);
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

const BladeColor& getCurrentBladeColor() {
  return BLADE_COLORS[currentColorIndex];
}
