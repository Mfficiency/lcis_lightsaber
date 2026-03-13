// STEP 13: Brightness Control
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D2 -> NeoPixel DIN
// XIAO ESP32-C3 5V/VBUS -> NeoPixel 5V
// XIAO ESP32-C3 GND -> NeoPixel GND

#include <Adafruit_NeoPixel.h>

const int LED_PIN = 2;
const int LED_COUNT = 60;
const int BRIGHTNESS = 80;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  turnBladeOn();
}

void loop() {
}

void turnBladeOn() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 255));
  }

  strip.show();
}
