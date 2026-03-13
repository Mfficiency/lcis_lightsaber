// STEP 05: Read a Button
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D0 -> pushbutton -> GND
// Uses INPUT_PULLUP, so the pin reads LOW when pressed

const int BUTTON_PIN = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {
    Serial.println("Button pressed");
  } else {
    Serial.println("Button released");
  }

  delay(250);
}
