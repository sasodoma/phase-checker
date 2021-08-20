uint16_t phase_time = 0;
uint16_t old_phase_time = 0;
uint8_t phase_goal = 0;
boolean flag = false;

const char* phase_names[] = {"+L1", "-L3", "+L2", "-L1", "+L3", "-L2"};

void setup() {
  // TCCR1A/B
  // Output mode - none
  // COM1 A/B 1/0 - 0

  // CTC mode, TOP is OCR1A
  // WGM: 13-0, 12-1, 11-0, 10-0

  // Noise cancel: ICNC1-1
  // Edge select: ICES1-0

  // clk/8
  // CS: 12-0, 11-1, 10-0
  TCCR1A = B01000000;
  TCCR1B = B10001010;
  OCR1A = 40000;

  // TIMSK1
  // Input capture interrupt enable
  // ICIE1 - 1
  TIMSK1 = B00100001;

  Serial.begin(115200);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, OUTPUT);
}

void loop() {
  uint16_t modulo = phase_time % (OCR1A / 6);
  uint8_t new_phase_goal = phase_time / (OCR1A / 6);
  if (modulo > OCR1A / 12) new_phase_goal++;
  phase_goal = new_phase_goal < 6 ? new_phase_goal : 0;
  if (flag) {
    flag = false;
    Serial.print(phase_names[phase_goal]);
    Serial.print("\t");
    Serial.print(phase_time);
    Serial.print("\t");
    Serial.println(OCR1A);
  }
}

ISR(TIMER1_CAPT_vect) {
  phase_time = ICR1L;
  phase_time |= ICR1H << 8;
  uint16_t phase_time_goal = (OCR1A / 6) * phase_goal;
  uint16_t pos_dif, neg_dif;
  if (phase_time_goal > phase_time) {
    pos_dif = OCR1A - phase_time_goal + phase_time;
    neg_dif = phase_time_goal - phase_time;
  } else {
    pos_dif = phase_time - phase_time_goal;
    neg_dif = OCR1A - phase_time + phase_time_goal;
  }
  bool dir = phase_time > old_phase_time;
  bool need_dir = neg_dif < pos_dif;
  if (dir != need_dir) {
    OCR1A += dir ? 1 : -1;
  }
  old_phase_time = phase_time;
  flag = true;
}
