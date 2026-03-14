// STEP 09: Led Strip On and Off
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
  Serial.println("Led Strip on/off demo ready.");
}

void loop() {
  bladeOn();
  Serial.println("Led Strip ON");
  delay(1000);

  bladeOff();
  Serial.println("Led Strip OFF");
  delay(1000);
}

void bladeOn() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }

  strip.show();
}

void bladeOff() {
  strip.clear();
  strip.show();
}

