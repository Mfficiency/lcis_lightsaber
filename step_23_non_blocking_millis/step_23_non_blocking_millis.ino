// STEP 23: Non-Blocking Timing with Millis

#include <Adafruit_NeoPixel.h>

const int BUTTON_PIN = D0;
const int BLADE_PIN = D0;
const int LED_COUNT = 60;
const unsigned long STEP_DELAY_MS = 30;

bool saberOn = false;
bool animationRunning = false;
int litPixels = 0;
int lastButtonState = HIGH;
unsigned long lastUpdate = 0;

Adafruit_NeoPixel strip(LED_COUNT, BLADE_PIN, NEO_GRB + NEO_KHZ800);

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
