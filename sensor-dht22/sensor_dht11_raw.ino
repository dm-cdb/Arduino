/* This is an ultra basic code to read a DHT11 sensor
 * The code should compile and use under 3kB of flash
 * Christian de Balorre, December 22nd, 2025
 */

#define DHT_PIN 2

uint8_t data[5]; // Humidity on 2 bytes, Temp on 2 bytes, checksum 1 byte
uint8_t negative;
unsigned long start;
char sign;

bool readDHT11() {
  uint8_t byteIdx = 0;
  uint8_t bitIdx = 7;

  // Clear data buffer
  for (int i = 0; i < 5; i++) data[i] = 0;

  pinMode(DHT_PIN, INPUT);
  delay(1);

  // === Start Signal ===
  pinMode(DHT_PIN, OUTPUT);
  digitalWrite(DHT_PIN, LOW);
  delay(20);  // MCU START with LOW ≥18ms
  pinMode(DHT_PIN, INPUT);
  
  // === DHT should ACK with a LOW within 20µs < ACK < 40µs ===
  start = micros();
  delayMicroseconds((start + 40) - start); 
  if (digitalRead(DHT_PIN)) {
    Serial.println("DHT not ready");
    return false;
  }


  // === DHT ACK Response 83µs LOW + 87µs HIGH ===
  // === This code uses the Arduino pulseIn function to measure the HIGH pulse duration ===
    if (pulseIn(DHT_PIN, HIGH, 200) == 0){
   Serial.println("DHT invalid ACK");
   return false; 
  } 
    
  // === Read 40 bits ===
  for (int i = 0; i < 40; i++) {
    
    // === Measure HIGH duration ; wait for pulse within 50µs LOW + 28µs (0) | 70µs (1) HIGH ===
    unsigned long highTime = pulseIn(DHT_PIN, HIGH,200);
    if (highTime == 0) {
      Serial.println("Data high state too long");
      return false;
    }

    // === HIGH > ~40µs means 1 ===
    if (highTime > 40) {
      data[byteIdx] |= (1 << bitIdx);
    }

    // === Decrement bitIDx until bitIdx == 0 ; DHT start with MSB ===
 
    if (bitIdx == 0) {
      bitIdx = 7;
      byteIdx++;
    } else {
      bitIdx--;
    }
  }

  // === Checksum ===
  uint8_t checksum = data[0] + data[1] + data[2] + data[3];
  return (checksum == data[4]);
}

void setup() {
  Serial.begin(9600);
  // === Wait for sensor to stabilize ===
  delay(3000);
}

void loop() {
  if (readDHT11()) {
    negative = (data[3]>>7);
    // === If negative temp, msb of data[3] set to 1 ===
    if (negative) {
      sign='-';      
      data[3] = data[3] & 0x7f; // Remove negative bit
    } else {
      sign = '+';
    }
    Serial.print("Humidity: ");
    Serial.print(data[0]);
    Serial.print(" %  ");

    Serial.print("Temperature: ");
    Serial.print (sign);
    Serial.print(data[2]);
    Serial.print(",");
      Serial.print(data[3]);
    Serial.println(" °C");
  } else {
    Serial.println("DHT11 read failed");
  }

  delay(3000);  // Minimum delay between reads
}
