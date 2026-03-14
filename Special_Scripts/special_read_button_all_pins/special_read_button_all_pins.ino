// SPECIAL: Read Button on All Candidate Pins
//
// WIRING DIAGRAM
// Connect one side of the pushbutton to GND
// Connect the other side of the pushbutton to one test pin at a time:
// D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, or D10
//
// This sketch enables INPUT_PULLUP on each candidate pin.
// Unpressed pins read HIGH (RELEASED).
// A pressed button connected to GND reads LOW (PRESSED).

struct ButtonPin {
  const char* name;
  int pin;
};

const ButtonPin BUTTON_PINS[] = {
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

const int PIN_COUNT = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);
const unsigned long PRINT_INTERVAL_MS = 250;

unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  for (int i = 0; i < PIN_COUNT; i++) {
    pinMode(BUTTON_PINS[i].pin, INPUT_PULLUP);
  }

  Serial.println("Button pin scan started");
  Serial.println("Wire the button between GND and one pin at a time.");
  Serial.println("PRESSED means the pin is reading LOW.");
  Serial.println();
}

void loop() {
  if (millis() - lastPrintTime >= PRINT_INTERVAL_MS) {
    lastPrintTime = millis();
    printPinStates();
  }
}

void printPinStates() {
  bool anyPressed = false;

  for (int i = 0; i < PIN_COUNT; i++) {
    int state = digitalRead(BUTTON_PINS[i].pin);
    bool pressed = (state == LOW);

    Serial.print(BUTTON_PINS[i].name);
    Serial.print(": ");

    if (pressed) {
      Serial.println("PRESSED");
      anyPressed = true;
    } else {
      Serial.println("RELEASED");
    }
  }

  if (!anyPressed) {
    Serial.println("No button press detected on any test pin.");
  }

  Serial.println();
}
