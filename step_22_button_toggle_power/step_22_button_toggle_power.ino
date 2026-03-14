// STEP 22: Button Toggles Saber Power

#include <Adafruit_NeoPixel.h>

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int BUTTON_PIN = 0;
const int LED_STRIP_PIN = 2;
#else
const int BUTTON_PIN = D7;
const int LED_STRIP_PIN = D8;
#endif

const int LED_COUNT = 60;
const int SAFE_BRIGHTNESS = 25;

bool saberOn = false;
int lastButtonState = HIGH;

Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.setBrightness(SAFE_BRIGHTNESS);
  strip.clear();
  strip.show();
  Serial.println("Button power control ready. Led Strip OFF");
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    saberOn = !saberOn;
    Serial.println("Button pressed");

    if (saberOn) {
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
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }

  strip.show();
}

void turnBladeOff() {
  strip.clear();
  strip.show();
}


