#include <Adafruit_NeoPixel.h>
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

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGBW + NEO_KHZ800);

// button state
int lastButtonState = HIGH;
bool buttonHeld = false;

// gyro value
float currentTilt = 0.0;
float lastLoggedTilt = 0.0;

// status LED flicker
unsigned long lastBlink = 0;
bool blinkState = false;

// ------------------------------------------------------
// FORWARD DECLARATIONS
// ------------------------------------------------------

void readMPU();          
float getTiltValue();

void updateSensors();
void updateStatusLed();

void fillUp();
void emptyDown();

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
}

// ------------------------------------------------------
// MAIN LOOP
// ------------------------------------------------------

void loop() {
  fillUp();
  delay(500);
  emptyDown();
}

// ------------------------------------------------------
// ANIMATION
// ------------------------------------------------------

void fillUp() {
  unsigned long stepDelay = FILL_TIME_MS / LED_COUNT;

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 50, 0, 0)); // GRBW
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
// MPU PLACEHOLDERS
// ------------------------------------------------------

void readMPU() {
  // TODO: students implement gyro reading
}

float getTiltValue() {
  return currentTilt;
}