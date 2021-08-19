
void setup() {
  pinMode(PB0, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  static unsigned long lastMillis = 0;
  static boolean input_was_high = true;
  if (millis() - lastMillis > 3) {
    if (digitalRead(PB0)) {
      if (!input_was_high) {
        lastMillis = millis();
        input_was_high = true;
      }
    } else {
      if (input_was_high) {
        lastMillis = millis();
        uint16_t phase_time = micros() % 20000;
        Serial.print(map_float(phase_time, 0, 20000, 0, 360));
        Serial.println("°");
      }
      input_was_high = false;
    }
  }
}

float map_float(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
