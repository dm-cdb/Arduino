/* Arduino Nano Analog comparator with TIMER1 capture
* AIN0 → D6 (non inverting input) : set ref voltage to 2,5V
* AIN1 → D7 (inverting input) : set AC signal from 0 to 5V
* TIMER1 prescaler 1:8 => 16Mhz / 8 = 2MHz => 1 tick each 0,5 µs
* TCCR1 register overflow = 65535 * 0,5 = 33 ms
* @ 500Hz Period = 2ms = 4000 ticks = 16 periods per overflow
*
* Christian de Balorre January 14th 2026
*/

volatile uint16_t lastCapture = 0;
volatile uint16_t periodTicks = 0;
volatile bool newMeasurement = false;
uint8_t count = 0;

ISR(TIMER1_CAPT_vect) {
  uint16_t capture = ICR1;
  periodTicks = capture - lastCapture;
  lastCapture = capture;
  newMeasurement = true;
}

void setup() {
  Serial.begin(9600);

  // Disable digital input buffers on AIN pins
  DIDR1 |= (1 << AIN0D) | (1 << AIN1D);

  // ----- Analog Comparator -----
  ACSR = 0;
  ACSR |= (1 << ACIC);  // Route comparator output to Timer1 capture

  // ----- Timer1 -----
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS11);   // Prescaler = 8 → 2 MHz timer clock
  TCCR1B |= (1 << ICES1);  // Capture on rising edge
  TCCR1B |= (1 << ICNC1);  // Input capture noise canceller
  TCNT1 = 0;

  // Enable Timer1 input capture interrupt
  TIMSK1 |= (1 << ICIE1);

  sei();
}

void loop() {
  if (newMeasurement) {
    noInterrupts();
    uint16_t ticks = periodTicks;
    newMeasurement = false;
    interrupts();

    if (ticks > 0) {
      // 16 MHz / 8 → 0.5 µs per tick
      uint32_t period_us = ticks >> 1;
      uint32_t frequency = 2000000UL / ticks;

      if (count > 250) { // @500Hz = display result each 2ms * 250)
        count = 0;
        Serial.print("Frequency: ");
        Serial.print(frequency);
        Serial.println(" Hz");
        Serial.print("Period: ");
        Serial.print(period_us);
        Serial.println(" us");
      }
      count++;
    }
  }
}
