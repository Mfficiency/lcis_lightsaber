// STEP 27: Startup Sound

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int BUZZER_PIN = 10;
#else
const int BUZZER_PIN = D4;
#endif

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
