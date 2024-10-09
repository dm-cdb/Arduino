/* This is a refactoring of a 2011 code by iwi on netquote.it/nqmain/category/arduino/
 * It has been ported as is numerous time on various projects with the formula (ADC_read_value - 324.31) / 1.22 
 * This is not the correct formula, as per datasheet you need to add + 25. 
 * Beside the OFFSET (324.21) and GAIN (1.22) chosen are specifics to each device or serie.
 * I hope the adapted code below is more useful.
 * 
 * Christian de Balorre, October 9th, 2024
 */


double Tc;

double GetTemp(void) {
  unsigned int wADC;
  float t;
  
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3)); // Internal 1.1 ref voltage, channel 9
  ADCSRA |= _BV(ADEN);                           // Enable the ADC
  delay(10);                                     // wait for internal voltage to become stable.

  ADCSRA |= 1 << ADSC;                          // Start the ADC
  while (!bitRead(ADCSRA, ADIF));               // ADIF flag set when conversion is over and WADC written
  wADC = ADCW;                                 // Register "ADCW" takes care of how to read ADCL and ADCH.
  ADCW = 0;
 Serial.println(wADC);
 bitSet(ADCSRA, ADIF);

  // Cf Atmega328P datasheet formula p. 216 : 25 + [(ADCW – (273 + 100 – TS_OFFSET)) * 128] / TS_GAIN
  // TS_OFFSET = 56 ; TS_GAIN = 1.15 ; specific to each device, see readme.md
  t = ((float)(wADC - 317.0 ) / 1.15 ) + 25.0;  // T° in degrees Celcius.
  
  return (t);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  while (true) {
      Tc = GetTemp();
      Serial.print("C° : ");
      Serial.println(Tc);
      delay(3000);
    }
   
}
