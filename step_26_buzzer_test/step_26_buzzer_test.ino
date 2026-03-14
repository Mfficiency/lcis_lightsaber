// STEP 26: Buzzer Test

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int BUZZER_PIN = 10;
#else
const int BUZZER_PIN = D4;
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("Buzzer test ready.");
}

void loop() {
  Serial.println("Buzzer tone -> 880 Hz");
  tone(BUZZER_PIN, 880, 200);
  delay(600);
}
