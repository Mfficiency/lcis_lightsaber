// STEP 23: Non-Blocking Timing with Millis

#include <Adafruit_NeoPixel.h>

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int BUTTON_PIN = 0;
const int LED_STRIP_PIN = 2;
#else
const int BUTTON_PIN = D7;
const int LED_STRIP_PIN = D9;
#endif

const int LED_COUNT = 60;
const unsigned long STEP_DELAY_MS = 30;

bool saberOn = false;
bool animationRunning = false;
int litPixels = 0;
int lastButtonState = HIGH;
unsigned long lastUpdate = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.clear();
  strip.show();
}

void loop() {
  handleButton();
  updateBladeAnimation();
}

void handleButton() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH && !animationRunning) {
    saberOn = !saberOn;
    animationRunning = true;

    if (saberOn) {
      litPixels = 0;
    } else {
      litPixels = LED_COUNT;
    }
  }

  lastButtonState = buttonState;
}

void updateBladeAnimation() {
  if (!animationRunning) {
    return;
  }

  unsigned long now = millis();
  if (now - lastUpdate < STEP_DELAY_MS) {
    return;
  }

  lastUpdate = now;

  if (saberOn) {
    strip.setPixelColor(litPixels, strip.Color(0, 0, 255));
    litPixels++;
    strip.show();

    if (litPixels >= LED_COUNT) {
      animationRunning = false;
    }
  } else {
    litPixels--;
    strip.setPixelColor(litPixels, 0);
    strip.show();

    if (litPixels <= 0) {
      animationRunning = false;
    }
  }
}
