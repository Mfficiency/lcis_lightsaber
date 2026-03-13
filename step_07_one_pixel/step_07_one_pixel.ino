// STEP 07: Turn On One Pixel of the LED Strip
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
  strip.setPixelColor(0, strip.Color(0, 0, 255));
  strip.show();
}

void loop() {
}
