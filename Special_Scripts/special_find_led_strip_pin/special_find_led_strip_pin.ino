// SPECIAL: Find LED Strip Data Pin
//
// WIRING DIAGRAM
// XIAO ESP32-C3 5V/VBUS -> NeoPixel 5V
// XIAO ESP32-C3 GND -> NeoPixel GND
// NeoPixel DIN -> one candidate data pin at a time
//
// This sketch tests each candidate pin one by one.
// It turns on only pixel 0 for 500 ms, then turns it off for 500 ms.
// Watch the strip and note which test pin lights the first pixel.

#include <Adafruit_NeoPixel.h>

#define USE_SUPER_MINI_PINS 0

struct LedStripPin {
  const char* name;
  int pin;
};

#if USE_SUPER_MINI_PINS
const LedStripPin LED_STRIP_PINS[] = {
  {"2", 2},
  {"3", 3},
  {"4", 4},
  {"5", 5},
  {"6", 6},
  {"7", 7},
  {"8", 8},
  {"9", 9}
};
#else
const LedStripPin LED_STRIP_PINS[] = {
  {"D0", D0},
  {"D1", D1},
  {"D2", D2},
  {"D3", D3},
  {"D4", D4},
  {"D5", D5},
  {"D6", D6},
  {"D7", D7},
  {"D8", D8},
  {"D9", D9},
  {"D10", D10}
};
#endif

const int LED_COUNT = 60;
const int SAFE_BRIGHTNESS = 25;
const int PIN_COUNT = sizeof(LED_STRIP_PINS) / sizeof(LED_STRIP_PINS[0]);
const unsigned long TEST_DELAY_MS = 500;

void testPin(const LedStripPin& testPinInfo);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("LED strip pin scan started.");
  Serial.println("Watch for pixel 0 turning on.");
  Serial.println();
}

void loop() {
  for (int i = 0; i < PIN_COUNT; i++) {
    testPin(LED_STRIP_PINS[i]);
  }
}

void testPin(const LedStripPin& testPinInfo) {
  Adafruit_NeoPixel testStrip(LED_COUNT, testPinInfo.pin, NEO_GRBW + NEO_KHZ800);

  testStrip.begin();
  testStrip.setBrightness(SAFE_BRIGHTNESS);
  testStrip.clear();
  testStrip.show();

  Serial.print("Testing pin ");
  Serial.println(testPinInfo.name);

  testStrip.setPixelColor(0, testStrip.Color(255, 255, 255));
  testStrip.show();
  delay(TEST_DELAY_MS);

  testStrip.clear();
  testStrip.show();
  delay(TEST_DELAY_MS);
}
