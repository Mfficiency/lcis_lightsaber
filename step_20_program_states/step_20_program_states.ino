// STEP 20: Introduce Program States

enum SaberState {
  OFF_STATE,
  TURNING_ON_STATE,
  ON_STATE
};

SaberState currentState = OFF_STATE;
unsigned long stateStartTime = 0;

void setup() {
  Serial.begin(115200);
  stateStartTime = millis();
}

void loop() {
  unsigned long now = millis();

  if (currentState == OFF_STATE && now - stateStartTime > 2000) {
    currentState = TURNING_ON_STATE;
    stateStartTime = now;
    Serial.println("State: TURNING_ON");
  } else if (currentState == TURNING_ON_STATE && now - stateStartTime > 1000) {
    currentState = ON_STATE;
    stateStartTime = now;
    Serial.println("State: ON");
  } else if (currentState == ON_STATE && now - stateStartTime > 2000) {
    currentState = OFF_STATE;
    stateStartTime = now;
    Serial.println("State: OFF");
  }
}
