// STEP 06: Turn On One Pixel of the LED Strip
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D9 -> NeoPixel DIN
// XIAO ESP32-C3 5V/VBUS -> NeoPixel 5V
// XIAO ESP32-C3 GND -> NeoPixel GND

#include <Adafruit_NeoPixel.h>

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int BUTTON_LED_PIN = 2;
#else
const int BUTTON_LED_PIN = D9;
#endif
const int LED_COUNT = 60;

Adafruit_NeoPixel strip(LED_COUNT, BUTTON_LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 0, 255));
  strip.show();
  Serial.println("Led Strip pixel 0 ON");
}

void loop() {
}
