// STEP 03: Blink an LED
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D10 -> 220-330 ohm resistor -> LED anode (+)
// LED cathode (-) -> GND
//
// Seeed Studio XIAO ESP32-C3 does not have a programmable user LED.
// Connect an external LED to D10 through a 220-330 ohm resistor, then to GND.
// 220 ohm is a good default for most standard 3 mm or 5 mm LEDs.
const int LED_PIN = D10;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);

  digitalWrite(LED_PIN, LOW);
  delay(500);
}
