// STEP 12: Shutdown Animation
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D0 -> NeoPixel DIN
// XIAO ESP32-C3 5V/VBUS -> NeoPixel 5V
// XIAO ESP32-C3 GND -> NeoPixel GND

#include <Adafruit_NeoPixel.h>

const int LED_PIN = D0;
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
  shutdownBlade();
  delay(1000);
}

void igniteBlade() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 255));
    strip.show();
    delay(30);
  }
}

void shutdownBlade() {
  for (int i = LED_COUNT - 1; i >= 0; i--) {
    strip.setPixelColor(i, 0);
    strip.show();
    delay(30);
  }
}
