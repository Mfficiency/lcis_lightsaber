// STEP 07: Turn On One Pixel of the LED Strip

#include <Adafruit_NeoPixel.h>

const int LED_PIN = 2;
const int LED_COUNT = 60;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 0, 255));
  strip.show();
}

void loop() {
}
