// STEP 27: Startup Sound

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int BUZZER_PIN = 10;
#else
const int BUZZER_PIN = D5;
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("Startup sound start");
  playStartupSound();
  Serial.println("Startup sound complete");
}

void loop() {
}

void playStartupSound() {
  Serial.println("Tone -> 440 Hz");
  tone(BUZZER_PIN, 440, 120);
  delay(150);
  Serial.println("Tone -> 660 Hz");
  tone(BUZZER_PIN, 660, 120);
  delay(150);
  Serial.println("Tone -> 880 Hz");
  tone(BUZZER_PIN, 880, 180);
  delay(220);
}
