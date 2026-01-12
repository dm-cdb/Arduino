/* Arduino Nano Analog comparator
* AIN0 → D6 (non inverting input)
* AIN1 → D7 (inverting input)
* Christian de Balorre Monday 12th January
*/

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  // Disable digital input buffers on AIN0 and AIN1 (reduces noise & power)
  DIDR1 = (1 << AIN0D) | (1 << AIN1D);

  // Analog Comparator setup:
  // ACBG = 0 → use AIN1 as negative input
  // ACIE = 0 → no interrupt
  // ACIC = 0 → not connected to Timer1
  // ACIS1:0 = 00 → comparator enabled, interrupt disabled anyway
  ACSR = 0x00;

  Serial.println("Analog Comparator ready");
}

void loop() {
  if (ACSR & (1 << ACO)) {
    Serial.println("AIN0 > AIN1");
    digitalWrite(3, 0);
    digitalWrite(2, 1);
    
  } else {
    Serial.println("AIN0 < AIN1");
    digitalWrite(3, 1);
    digitalWrite(2, 0);
  }

  delay(500);
}