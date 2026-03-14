// STEP 08: Change Led Strip Color
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

int redValue = 255;
int greenValue = 0;
int blueValue = 0;

Adafruit_NeoPixel strip(LED_COUNT, BUTTON_LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  strip.begin();
  Serial.println("Led Strip color demo ready.");
}

void loop() {
  fillBlade(redValue, greenValue, blueValue);
  Serial.println("Led Strip color -> RED");
  delay(1000);

  fillBlade(0, 255, 0);
  Serial.println("Led Strip color -> GREEN");
  delay(1000);

  fillBlade(0, 0, 255);
  Serial.println("Led Strip color -> BLUE");
  delay(1000);
}

void fillBlade(int red, int green, int blue) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }

  strip.show();
}
