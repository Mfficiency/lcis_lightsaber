// STEP 06: Button Controls an LED
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D0 -> pushbutton -> GND
// XIAO ESP32-C3 D8 -> 220-330 ohm resistor -> LED anode (+)
// LED cathode (-) -> GND
// Button uses INPUT_PULLUP, so the pin reads LOW when pressed

const int BUTTON_PIN = 0;
const int LED_PIN = 8;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}
