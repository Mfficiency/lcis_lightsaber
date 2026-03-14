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

const unsigned long DEBOUNCE_MS = 200;

bool ledOn = false;
int lastButtonState = HIGH;
unsigned long lastPressTime = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LED_PIN, OUTPUT);
  Serial.begin(115200);
  digitalWrite(BUTTON_LED_PIN, LOW);
  Serial.println("Button LED toggle ready. LED OFF");
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  if (buttonState == LOW && lastButtonState == HIGH && now - lastPressTime >= DEBOUNCE_MS) {
    ledOn = !ledOn;
    lastPressTime = now;

    if (ledOn) {
      digitalWrite(BUTTON_LED_PIN, HIGH);
      Serial.println("Button pressed -> LED ON");
    } else {
      digitalWrite(BUTTON_LED_PIN, LOW);
      Serial.println("Button pressed -> LED OFF");
    }
  }

  lastButtonState = buttonState;
  delay(20);
}
