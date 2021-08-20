uint16_t phase_time = 0;
uint16_t old_phase_time = 0;
uint8_t phase_goal = 0;
boolean flag = false;

boolean offline = false;
boolean got_pulse_last_period = true;
uint8_t list_index = 0;
uint16_t period_over_time[256] = {0};
uint8_t remainder = 0;
uint8_t loop_counter = 0;


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
  TIMSK1 = B00100010;

  Serial.begin(115200);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, OUTPUT);
  pinMode(2, OUTPUT);
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
  if (offline) {
    uint32_t total = 0;
    for (uint16_t i = 0; i < 256; i++) {
      total += period_over_time[i];
    }
    OCR1A = (total >> 8) + (loop_counter < total % 256);
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
  period_over_time[list_index++ % 256] = OCR1A;
  got_pulse_last_period = true;
  flag = true;
}

ISR(TIMER1_COMPA_vect) {
  loop_counter++;
  if (got_pulse_last_period) {
    PORTD |= B00000100;
    offline = false;
    got_pulse_last_period = false;
  } else {
    PORTD &= B11111011;
    offline = true;
  }
}
