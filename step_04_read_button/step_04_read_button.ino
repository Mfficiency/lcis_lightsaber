// STEP 04: Read a Button
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D7 -> pushbutton -> GND
// Uses INPUT_PULLUP, so the pin reads LOW when pressed

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int BUTTON_PIN = 0;
#else
const int BUTTON_PIN = D7;
#endif

int lastButtonState = HIGH;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("Button monitor ready.");
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      Serial.println("Button pressed");
    } else {
      Serial.println("Button released");
    }
  }

  lastButtonState = buttonState;
  delay(20);
}
