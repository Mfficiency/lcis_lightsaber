// STEP 02: Serial Hello World

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("Hello from the ESP32-C3.");
  delay(1000);
}
