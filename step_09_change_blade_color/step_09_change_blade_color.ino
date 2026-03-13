// STEP 09: Change Blade Color
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D0 -> NeoPixel DIN
// XIAO ESP32-C3 5V/VBUS -> NeoPixel 5V
// XIAO ESP32-C3 GND -> NeoPixel GND

#include <Adafruit_NeoPixel.h>

const int LED_PIN = D0;
const int LED_COUNT = 60;

int redValue = 255;
int greenValue = 0;
int blueValue = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
}

void loop() {
  fillBlade(redValue, greenValue, blueValue);
  delay(1000);

  fillBlade(0, 255, 0);
  delay(1000);

  fillBlade(0, 0, 255);
  delay(1000);
}

void fillBlade(int red, int green, int blue) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }

  strip.show();
}
