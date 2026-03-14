// STEP 20: Idle Led Strip Effect

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
  randomSeed(micros());
  Serial.println("Idle Led Strip effect running.");
}

void loop() {
  int baseBlue = 160;
  int flicker = random(-20, 21);
  int finalBlue = constrain(baseBlue + flicker, 80, 255);

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, finalBlue));
  }

  strip.show();
  delay(60);
}


