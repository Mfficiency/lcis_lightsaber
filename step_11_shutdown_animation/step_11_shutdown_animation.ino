// STEP 11: Shutdown Animation
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
const int SHUTDOWN_PEAK_BRIGHTNESS = 60;
const int BLADE_RED = 0;
const int BLADE_GREEN = 100;
const int BLADE_BLUE = 0;
const int BLADE_WHITE = 100;

Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRBW + NEO_KHZ800);

void pulseBrightness();

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(SAFE_BRIGHTNESS);
  strip.clear();
  strip.show();
  Serial.println("Shutdown demo ready.");
}

void loop() {
  Serial.println("Ignition start");
  igniteBlade();
  Serial.println("Led Strip fully ON");
  delay(1000);
  Serial.println("Shutdown start");
  shutdownBlade();
  Serial.println("Led Strip OFF");
  delay(1000);
}

void igniteBlade() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(BLADE_RED, BLADE_GREEN, BLADE_BLUE, BLADE_WHITE));
    strip.show();
    delay(30);
  }
}

void shutdownBlade() {
  pulseBrightness();

  int leftIndex = (LED_COUNT - 1) / 2;
  int rightIndex = LED_COUNT / 2;

  while (leftIndex >= 0 || rightIndex < LED_COUNT) {
    if (leftIndex >= 0) {
      strip.setPixelColor(leftIndex, 0);
    }

    if (rightIndex < LED_COUNT) {
      strip.setPixelColor(rightIndex, 0);
    }

    strip.show();
    delay(30);

    leftIndex--;
    rightIndex++;
  }

  strip.setBrightness(SAFE_BRIGHTNESS);
  strip.clear();
  strip.show();
}

void pulseBrightness() {
  for (int brightness = SAFE_BRIGHTNESS; brightness <= SHUTDOWN_PEAK_BRIGHTNESS; brightness += 5) {
    strip.setBrightness(brightness);
    strip.show();
    delay(15);
  }

  for (int brightness = SHUTDOWN_PEAK_BRIGHTNESS; brightness >= SAFE_BRIGHTNESS; brightness -= 5) {
    strip.setBrightness(brightness);
    strip.show();
    delay(35);
  }
}
