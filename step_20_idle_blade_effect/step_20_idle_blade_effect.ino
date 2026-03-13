// STEP 20: Idle Blade Effect

#include <Adafruit_NeoPixel.h>

const int BLADE_PIN = D0;
const int LED_COUNT = 60;

Adafruit_NeoPixel strip(LED_COUNT, BLADE_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  randomSeed(micros());
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
