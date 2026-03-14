// STEP 05: Button Controls an LED
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D7 -> pushbutton -> GND
// XIAO ESP32-C3 D2 -> 220-330 ohm resistor -> LED anode (+)
// LED cathode (-) -> GND
// Button uses INPUT_PULLUP, so the pin reads LOW when pressed

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int BUTTON_PIN = 0;
const int BUTTON_LED_PIN = 7;
#else
const int BUTTON_PIN = D7;
const int BUTTON_LED_PIN = D2;
#endif

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Button LED control ready.");
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {
    digitalWrite(BUTTON_LED_PIN, HIGH);
    Serial.println("Button pressed -> LED ON");
  } else {
    digitalWrite(BUTTON_LED_PIN, LOW);
    Serial.println("Button released -> LED OFF");
  }

  delay(20);
}
