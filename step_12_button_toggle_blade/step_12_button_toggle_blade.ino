// STEP 12: Button Toggles Led Strip On and Off
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D7 -> pushbutton -> GND
// XIAO ESP32-C3 D9 -> NeoPixel DIN
// XIAO ESP32-C3 5V/VBUS -> NeoPixel 5V
// XIAO ESP32-C3 GND -> NeoPixel GND

#include <Adafruit_NeoPixel.h>

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int BUTTON_PIN = 0;
const int LED_STRIP_PIN = 2;
#else
const int BUTTON_PIN = D7;
const int LED_STRIP_PIN = D9;
#endif

const int LED_COUNT = 60;

bool stripOn = false;
int lastButtonState = HIGH;

Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.clear();
  strip.show();
  Serial.println("Button toggle blade ready. Led Strip OFF");
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    stripOn = !stripOn;
    Serial.println("Button pressed");

    if (stripOn) {
      turnBladeOn();
      Serial.println("Led Strip ON");
    } else {
      turnBladeOff();
      Serial.println("Led Strip OFF");
    }
  }

  lastButtonState = buttonState;
  delay(20);
}

void turnBladeOn() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 255));
  }

  strip.show();
}

void turnBladeOff() {
  strip.clear();
  strip.show();
}
