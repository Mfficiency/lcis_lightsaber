// STEP 06: Turn On One Pixel of the LED Strip
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D8 -> NeoPixel DIN
// XIAO ESP32-C3 5V/VBUS -> NeoPixel 5V
// XIAO ESP32-C3 GND -> NeoPixel GND

#include <Adafruit_NeoPixel.h>

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int LED_STRIP_PIN = 2;
#else
const int LED_STRIP_PIN = D8;
#endif
const int LED_COUNT = 60;
const int SAFE_BRIGHTNESS = 25;

Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(SAFE_BRIGHTNESS);
  strip.clear();
  strip.show();
  Serial.println("Pixel 0 blink demo ready at 10 percent power.");
}

void loop() {
  strip.setPixelColor(0, strip.Color(255, 0, 0, 255));
  strip.show();
  Serial.println("Pixel 0 ON");
  delay(500);

  strip.setPixelColor(0, 0);
  strip.show();
  Serial.println("Pixel 0 OFF");
  delay(500);
}
