// STEP 10: Blade On and Off

#include <Adafruit_NeoPixel.h>

const int LED_PIN = 2;
const int LED_COUNT = 60;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
}

void loop() {
  bladeOn();
  delay(1000);

  bladeOff();
  delay(1000);
}

void bladeOn() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 255));
  }

  strip.show();
}

void bladeOff() {
  strip.clear();
  strip.show();
}
