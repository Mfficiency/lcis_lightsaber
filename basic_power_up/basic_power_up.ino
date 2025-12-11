#include <Adafruit_NeoPixel.h>
#include <math.h>
// Include your MPU library here
// #include <Wire.h>
// #include <MPU6050.h>

// ------------------------------------------------------
// CONFIG
// ------------------------------------------------------

#define LED_PIN        2
#define LED_COUNT      60
#define LED_BRIGHTNESS 80

const int BUTTON_PIN = 0;            // button to GND, use internal pullup
const int INT_LED_PIN = 8;           // built in LED
const int BUZZER_PIN = 10;           // NEW: buzzer for end beep

const unsigned long FILL_TIME_MS   = 2000;   // 2 seconds up
const unsigned long EMPTY_TIME_MS  = 2000;   // 2 seconds down
const float GYRO_THRESHOLD = 5.0;           // degrees change to log

// ------------------------------------------------------
// GLOBALS
// ------------------------------------------------------

// GRBW strip
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

// button state
int lastButtonState = HIGH;
bool buttonHeld = false;

// gyro value
float currentTilt = 0.0;
float lastLoggedTilt = 0.0;

// status LED flicker
unsigned long lastBlink = 0;
bool blinkState = false;

// color cycling
const int NUM_COLORS = 4;
uint32_t colors[NUM_COLORS];
int currentColorIndex = 0;
uint32_t currentColor = 0;

// orientation mode flag (falling ball)
bool orientationMode = false;

// NEW: timer mode flag
bool timerMode = false;

// ------------------------------------------------------
// FORWARD DECLARATIONS
// ------------------------------------------------------

void readMPU();
float getTiltValue();

void updateSensors();
void updateStatusLed();

void fillUp();
void emptyDown();

void showOrientation();   // falling ball animation
void showTimerMode();     // NEW: hold to fill, release to countdown

void beepEnd();           // NEW: small beep at end

// ------------------------------------------------------
// SETUP
// ------------------------------------------------------

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(INT_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);       // NEW
  digitalWrite(BUZZER_PIN, LOW);     // NEW

  strip.begin();
  strip.setBrightness(LED_BRIGHTNESS);
  strip.show();

  Serial.begin(115200);

  // GRBW logical colors: Color(r, g, b, w)
  colors[1] = strip.Color(0, 50, 0, 40);   // green
  colors[0] = strip.Color(50, 0, 0, 40);   // red
  colors[2] = strip.Color(0, 0, 50, 40);   // blue
  colors[3] = strip.Color(0, 0, 0, 50);    // white

  currentColorIndex = 0;
  currentColor = colors[currentColorIndex];
}

// ------------------------------------------------------
// MAIN LOOP
// ------------------------------------------------------

void loop() {
  if (orientationMode) {
    updateSensors();
    updateStatusLed();
    showOrientation();
    delay(50);
  } else if (timerMode) {           // NEW: timer mode handling
    updateSensors();
    updateStatusLed();
    showTimerMode();
    delay(50);
  } else {
    fillUp();
    delay(500);
    emptyDown();
  }
}

// ------------------------------------------------------
// ANIMATION
// ------------------------------------------------------

void fillUp() {
  unsigned long stepDelay = FILL_TIME_MS / LED_COUNT;

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, currentColor);
    strip.show();

    updateSensors();
    updateStatusLed();

    delay(stepDelay);
  }
}

void emptyDown() {
  unsigned long stepDelay = EMPTY_TIME_MS / LED_COUNT;

  for (int i = LED_COUNT - 1; i >= 0; i--) {
    strip.setPixelColor(i, 0);
    strip.show();

    updateSensors();
    updateStatusLed();

    delay(stepDelay);

    if (i == 0) break;
  }
}

// ------------------------------------------------------
// SENSOR AND LOGIC
// ------------------------------------------------------

void updateSensors() {
  // button handling
  int state = digitalRead(BUTTON_PIN);

  if (state == LOW && lastButtonState == HIGH) {
    Serial.println("Button pressed");
    buttonHeld = true;

    if (!orientationMode && !timerMode) {
      // go to next color or orientation mode (5th press)
      if (currentColorIndex < NUM_COLORS - 1) {
        currentColorIndex++;
        currentColor = colors[currentColorIndex];
      } else {
        orientationMode = true;  // 5th press
      }
    } else if (orientationMode) {
      // 6th press: leave orientation, enter timer mode
      orientationMode = false;
      timerMode = true;
    } else if (timerMode) {
      // next press: exit timer mode back to first color
      timerMode = false;
      currentColorIndex = 0;
      currentColor = colors[currentColorIndex];
    }
  }

  if (state == HIGH && lastButtonState == LOW) {
    buttonHeld = false;
  }

  lastButtonState = state;

  // gyro
  readMPU();
  float tilt = getTiltValue();
  if (fabs(tilt - lastLoggedTilt) > GYRO_THRESHOLD) {
    Serial.print("Gyro: ");
    Serial.println(tilt);
    lastLoggedTilt = tilt;
  }
}

// ------------------------------------------------------
// STATUS LED BEHAVIOR
// ------------------------------------------------------

void updateStatusLed() {
  unsigned long now = millis();

  // If button is held, LED stays fully on
  if (buttonHeld) {
    digitalWrite(INT_LED_PIN, HIGH);
    return;
  }

  // Otherwise flicker heartbeat
  if (now - lastBlink > 150) {
    lastBlink = now;
    blinkState = !blinkState;
    digitalWrite(INT_LED_PIN, blinkState ? HIGH : LOW);
  }
}

// ------------------------------------------------------
// ORIENTATION MODE: FALLING BALL ANIMATION
// ------------------------------------------------------
// Lights symmetric LEDs around the middle (29/30)
// and bounces between center and ends (0/59).

void showOrientation() {
  static int pos = 0;                     // 0..29, distance from center
  static int dir = 1;                     // +1 downwards, -1 upwards
  static unsigned long lastStep = 0;
  const unsigned long interval = 40;      // ms per step

  unsigned long now = millis();
  if (now - lastStep < interval) return;
  lastStep = now;

  strip.clear();

  int left  = 29 - pos;
  int right = 30 + pos;

  if (left >= 0) strip.setPixelColor(left, strip.Color(0, 0, 0, 50));    // white on GRBW
  if (right < LED_COUNT) strip.setPixelColor(right, strip.Color(0, 0, 0, 50));

  strip.show();

  pos += dir;

  if (pos >= 29) {
    pos = 29;
    dir = -1;
  } else if (pos <= 0) {
    pos = 0;
    dir = 1;
  }
}

// ------------------------------------------------------
// TIMER MODE: ends marker, timed fill, timed countdown
// ------------------------------------------------------

void showTimerMode() {
  static bool counting = false;
  static bool filling = false;
  static int ledsLit = 0;
  static unsigned long lastStep = 0;
  static unsigned long pressStart = 0;

  const unsigned long holdThreshold = 1000;      // 1 second to start filling
  const unsigned long fillInterval  = 333;       // 3 LEDs per second
  const unsigned long stepInterval  = 1000;      // 1 LED per second countdown

  unsigned long now = millis();

  // If we are counting down
  if (counting) {
    if (now - lastStep >= stepInterval) {
      lastStep += stepInterval;
      if (ledsLit > 0) {
        ledsLit--;
      }

      strip.clear();
      for (int i = 0; i < ledsLit; i++) {
        strip.setPixelColor(i, strip.Color(50, 0, 0, 0)); // red
      }
      strip.show();

      if (ledsLit == 0) {
        counting = false;
        beepEnd();
        // back to idle: show only first and last red
        strip.clear();
        strip.setPixelColor(0, strip.Color(50, 0, 0, 0));
        strip.setPixelColor(LED_COUNT - 1, strip.Color(50, 0, 0, 0));
        strip.show();
      }
    }
    return;
  }

  // Not counting: handle button-held logic
  if (buttonHeld) {
    // track how long the button has been held
    if (pressStart == 0) {
      pressStart = now;
    }

    if (!filling) {
      // wait until held > 1s before starting to fill
      if (now - pressStart >= holdThreshold) {
        filling = true;
        lastStep = now;
      }

      // still waiting: just show first and last red
      strip.clear();
      strip.setPixelColor(0, strip.Color(50, 0, 0, 0));
      strip.setPixelColor(LED_COUNT - 1, strip.Color(50, 0, 0, 0));
      strip.show();
    } else {
      // actively filling at 3 LEDs/s
      if (now - lastStep >= fillInterval) {
        lastStep += fillInterval;
        if (ledsLit < LED_COUNT) {
          ledsLit++;
        }
      }

      strip.clear();
      for (int i = 0; i < ledsLit; i++) {
        strip.setPixelColor(i, strip.Color(50, 0, 0, 0)); // red
      }
      strip.show();
    }

  } else {
    // button not held
    if (filling && ledsLit > 0) {
      // we were filling, user released: start countdown
      filling = false;
      counting = true;
      pressStart = 0;
      lastStep = now;
      // strip content already reflects ledsLit
    } else {
      // idle: only first and last red, waiting for a hold
      pressStart = 0;
      filling = false;
      ledsLit = 0;
      strip.clear();
      strip.setPixelColor(0, strip.Color(50, 0, 0, 0));
      strip.setPixelColor(LED_COUNT - 1, strip.Color(50, 0, 0, 0));
      strip.show();
    }
  }
}

// ------------------------------------------------------
// BEEP AT END OF TIMER
// ------------------------------------------------------

void beepEnd() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

// ------------------------------------------------------
// MPU PLACEHOLDERS
// ------------------------------------------------------

void readMPU() {
  // TODO: students implement gyro reading and set currentTilt
}

float getTiltValue() {
  return currentTilt;
}
