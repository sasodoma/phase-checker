uint8_t phase = 0;

int off_time = 0;
int on_time = 2;

void setup() {
  pinMode(PC13, OUTPUT);
  pinMode(PA0, INPUT_PULLUP);
  digitalWrite(PC13, HIGH);
  
  Serial.begin(115200);
  printPhase(phase);
}

void loop() {
  static uint16_t off_time = 0;
  static uint16_t on_time = 2000;

  uint16_t phase_time = micros() % 20000;
  digitalWrite(PC13, phase_time < off_time || phase_time > on_time);
  
  static unsigned long lastMillis = 0;
  static boolean key_depressed = true;
  if (millis() - lastMillis > 10) {
    if (digitalRead(PA0)) {
      if (!key_depressed) {
        lastMillis = millis();
        key_depressed = true;
      }
    } else {
      if (key_depressed) {
        lastMillis = millis();
        phase++;
        if (phase > 5) phase = 0;
        printPhase(phase);
        off_time = phase * 3333;
        on_time = off_time + 2000;
      }
      key_depressed = false;
    }
  }
}

void printPhase(uint8_t phase) {
  Serial.print("Setting phase ");
  Serial.print(phase * 60);
  Serial.println("°");
}
