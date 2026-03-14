// STEP 22: Button Toggles Saber Power

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

bool saberOn = false;
int lastButtonState = HIGH;

Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.clear();
  strip.show();
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    saberOn = !saberOn;

    if (saberOn) {
      turnBladeOn();
    } else {
      turnBladeOff();
    }
  }

  lastButtonState = buttonState;
  delay(20);
}

void turnBladeOn() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 255));
  }

  strip.show();
}

void turnBladeOff() {
  strip.clear();
  strip.show();
}
