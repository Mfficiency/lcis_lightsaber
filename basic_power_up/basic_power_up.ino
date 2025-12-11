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

// orientation mode flag
bool orientationMode = false;

// ------------------------------------------------------
// FORWARD DECLARATIONS
// ------------------------------------------------------

void readMPU();
float getTiltValue();

void updateSensors();
void updateStatusLed();

void fillUp();
void emptyDown();

void showOrientation();   // now: falling ball animation

// ------------------------------------------------------
// SETUP
// ------------------------------------------------------

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(INT_LED_PIN, OUTPUT);

  strip.begin();
  strip.setBrightness(LED_BRIGHTNESS);
  strip.show();

  Serial.begin(115200);

  // GRBW logical colors: Color(r, g, b, w)
  colors[0] = strip.Color(0, 50, 0, 40);   // green
  colors[1] = strip.Color(50, 0, 0, 40);   // red
  colors[2] = strip.Color(0, 0, 50, 40);   // blue
  colors[3] = strip.Color(0, 0, 0, 50);   // white

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

    if (!orientationMode) {
      // go to next color or orientation mode
      if (currentColorIndex < NUM_COLORS - 1) {
        currentColorIndex++;
        currentColor = colors[currentColorIndex];
      } else {
        // 5th press: enter orientation (falling ball) mode
        orientationMode = true;
      }
    } else {
      // exit orientation mode, back to first color
      orientationMode = false;
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
// MPU PLACEHOLDERS
// ------------------------------------------------------

void readMPU() {
  // TODO: students implement gyro reading and set currentTilt
}

float getTiltValue() {
  return currentTilt;
}
