// STEP 11: Ignition Animation

#include <Adafruit_NeoPixel.h>

const int LED_PIN = 2;
const int LED_COUNT = 60;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.clear();
  strip.show();
}

void loop() {
  igniteBlade();
  delay(1000);
  strip.clear();
  strip.show();
  delay(1000);
}

void igniteBlade() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 255));
    strip.show();
    delay(30);
  }
}
