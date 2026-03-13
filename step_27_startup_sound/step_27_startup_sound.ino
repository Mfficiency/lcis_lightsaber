// STEP 27: Startup Sound

const int BUZZER_PIN = 10;

void setup() {
  playStartupSound();
}

void loop() {
}

void playStartupSound() {
  tone(BUZZER_PIN, 440, 120);
  delay(150);
  tone(BUZZER_PIN, 660, 120);
  delay(150);
  tone(BUZZER_PIN, 880, 180);
  delay(220);
}
