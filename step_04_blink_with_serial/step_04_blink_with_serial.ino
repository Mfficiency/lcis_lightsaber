// STEP 04: Blink with Serial Messages

const int LED_PIN = 8;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("ON");
  delay(500);

  digitalWrite(LED_PIN, LOW);
  Serial.println("OFF");
  delay(500);
}
