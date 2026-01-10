/*This code show various way of setting GPIO registers using bitwise operators.
* Auth. Christian de Balorre
* January 10th 2026
*/

uint8_t status = 0;
uint8_t mask = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  DDRD |= 0b00010000;  // Set bit 4  (PD4) as OUTPUT - This is D4 on Arduino Nano
  // Bitwise HIGH - LOW
  Serial.println("Bitwise HIGH then LOW");
  for (uint8_t i = 0; i < 11; i++) {
    PORTD |= (1 << 4);  // Set bit 4 (PD4) (HIGH)
    delay(250);
    PORTD &= ~(1 << PD4);  // Clear bit 4 (PD4) (LOW)
    delay(250);
  }
  PORTD &= ~bit(PD4);  // Clear bit 4 (PD4) (LOW)
  delay(3000);

  // bit() HIGH and LOW
  Serial.println("Bit() HIGH then LOW");
  for (uint8_t i = 0; i < 11; i++) {
    PORTD |= bit(PD4);  // Set PORTD 4th bit to 1
    delay(250);
    PORTD &= ~bit(PD4);
    ;  // Set PORTD 4th bit to 0
    delay(250);
  }
  PORTD &= ~bit(PD4);
  delay(3000);

  // Toggle bitwise
  Serial.println("Toggle bitwise");
  for (uint8_t i = 0; i < 11; i++) {
    PORTD ^= (1 << PD4);
    delay(250);
  }
  PORTD &= ~bit(PD4);
  delay(3000);

  // Toggle magic formula
  Serial.println("Toggle magic formula");
  for (uint8_t i = 0; i < 11; i++) {
    PIND = bit(PD4);  // HIGH
    delay(250);
    PIND = bit(PD4);  //LOW
    delay(250);
  }
  PORTD &= ~bit(PD4);
  delay(3000);

  // Read input status on PD5 with PULLUP - This is D5 on Arduino Nano (left unconnected = HIGH ; grounded = LOW)
  Serial.println("Read INPUT D5 status with PULLUP");
  status = 0;
  mask = (1 << 5);
  DDRD &= ~mask;  // Force bit 5  (PD5) as INPUT
  PORTD |= mask;  // Activate PULLUP
  delay(5);
  status = (PIND & mask);
  if (status) {
    Serial.println("INPUT pin D5 HIGH");
  } else Serial.println("INPUT pin D5 LOW");
  delay(3000);
  PORTD &= ~mask;  // Deactivate PULLUP - floating
  DDRD = 0x00;     // Clear DDRD
  delay(3000);
}
