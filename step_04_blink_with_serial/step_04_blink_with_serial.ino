// STEP 04: Blink with Serial Messages
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D10 -> 220-330 ohm resistor -> LED anode (+) the long leg
// LED cathode (-) the short leg -> GND
//
// Seeed Studio XIAO ESP32-C3 does not have a programmable user LED.
// Connect an external LED to D10 through a 220-330 ohm resistor, then to GND.
// 220 ohm is a good default for most standard 3 mm or 5 mm LEDs.
const int BUTTON_LED_PIN = D10;

void setup() {
  pinMode(BUTTON_LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  digitalWrite(BUTTON_LED_PIN, HIGH);
  Serial.println("ON");
  delay(500);

  digitalWrite(BUTTON_LED_PIN, LOW);
  Serial.println("OFF");
  delay(500);
}
