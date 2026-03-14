// STEP 10: Ignition Animation
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D9 -> NeoPixel DIN
// XIAO ESP32-C3 5V/VBUS -> NeoPixel 5V
// XIAO ESP32-C3 GND -> NeoPixel GND

#include <Adafruit_NeoPixel.h>

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int LED_STRIP_PIN = 2;
#else
const int LED_STRIP_PIN = D9;
#endif
const int LED_COUNT = 60;
const int SAFE_BRIGHTNESS = 25;

Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(SAFE_BRIGHTNESS);
  strip.clear();
  strip.show();
  Serial.println("Ignition demo ready.");
}

void loop() {
  Serial.println("Ignition start");
  igniteBlade();
  Serial.println("Led Strip fully ON");
  delay(1000);
  strip.clear();
  strip.show();
  Serial.println("Led Strip cleared");
  delay(1000);
}

void igniteBlade() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255));
    strip.show();
    delay(30);
  }
}
