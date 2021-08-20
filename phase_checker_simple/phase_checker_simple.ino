
void setup() {
  pinMode(PB0, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  // Duration of one mains period in MCU microseconds (different from real time)
  static uint16_t period = 20000;
  static unsigned long cumulative_zero = 0;
  static unsigned long last_pulse = 0;
  
  static unsigned long last_millis = 0;
  static boolean input_was_high = true;
  // Update our relative zero time
  if (micros() - cumulative_zero > period) cumulative_zero += period;
  
  if (millis() - last_millis > 3) {
    if (digitalRead(PB0)) {
      if (!input_was_high) {
        last_millis = millis();
        input_was_high = true;
      }
    } else {
      if (input_was_high) {
        // Get both times at moment of pulse
        unsigned long micros_now = micros();
        last_millis = millis();
        // Calculate period
        unsigned long last_period = micros_now - last_pulse;
        // Save this pulse's time
        last_pulse = micros_now;

        // Update period
        period += last_period > period;
        period -= last_period < period;

        // Get phase relative to our current zero.
        uint16_t phase_time = (micros_now - cumulative_zero) % period;
        Serial.print(map_float(phase_time, 0, period, 0, 360));
        Serial.print("°, Period: ");
        Serial.println(period);
      }
      input_was_high = false;
    }
  }
}

float map_float(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
