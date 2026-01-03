/* This is an ultra basic code to read a DHT22 sensor
 * It uses the Arduino pulseIn() function to measure the duration of a HIGH pulse.
 * Please be aware pulseIn() function makes use of TIMER0 with interrupts ; do not disabled before !
 * The code should compile and use under 4,5kB of flash
 * Christian de Balorre, December 22nd, 2025
 */

#define DHT_PIN 2

uint8_t data[5]; // 2 Bytes for Hum, 2 Bytes for Temp, 1 Byte for checksum
uint8_t negative;
unsigned long start;
char sign;

bool readDHT22() {
   // Clear data buffer
  for (int i = 0; i < 5; i++) data[i] = 0;

  // === Start Signal ===
  pinMode(DHT_PIN, OUTPUT);
  digitalWrite(DHT_PIN, LOW);
  delay(4);  // MCU START with LOW ≥1ms
  pinMode(DHT_PIN, INPUT);

  // === DHT should ACK with a LOW within 20µs < ACK < 40µs ===
  start = micros();
  delayMicroseconds((start + 40) - start);
  if (digitalRead(DHT_PIN)) {
    Serial.println("DHT not ready");
    return false;
  }

  // === DHT ACK Response 83µs LOW + 87µs HIGH ===
  // === We use the Arduino pulsIn function here ===
  if (pulseIn(DHT_PIN, HIGH, 200) == 0) {
    Serial.println("DHT invalid ACK");
    return false;
  }
   noInterrupts();
  // === Compute the 40 bits stream of data sent by DHT22 sensors
  for (uint8_t Idx = 0; Idx < 5; Idx++) {
    for (int a = 0; a < 8; a++) {
      while (digitalRead(DHT_PIN) == LOW);
      // === If after 40µs DHT_PIN still HIGH, we have bit = 1 ===
      delayMicroseconds(40);
      if (digitalRead(DHT_PIN) == HIGH) {
        data[Idx] |= (1 << (7 - a));
      }
      while (digitalRead(DHT_PIN) == HIGH);
    }
  }
interrupts();
// === Checksum ===
uint8_t checksum = ((data[0] + data[1] + data[2] + data[3])) & 0xff;
return (checksum == data[4]);
}

void setup() {
  Serial.begin(9600);
  // === Wait for sensor to stabilize ===
  pinMode(DHT_PIN, INPUT);
  delay(3000);
}

void loop() {
  uint16_t resh, rest = 0;
  float Temp, Hum = 0;

  if (readDHT22()) {
    negative = (data[2] >> 7);
    // === If negative temp, msb of data[2] set to 1 ===
    if (negative) {
      sign = '-';
      data[2] = data[2] & 0x7f;  // Remove negative bit
    } else {
      sign = '+';
    }
    resh |= (uint16_t)(data[0] << 8);
    resh |= (uint16_t)data[1];
    Hum = resh / 10.0;

    rest |= (uint16_t)(data[2] << 8);
    rest |= (uint16_t)data[3];
    Temp = rest / 10.0;

    Serial.print("Humidity: ");
    Serial.print(Hum);
    Serial.print(" %  ");

    Serial.print("Temperature: ");
    Serial.print(sign);
    Serial.print(Temp);
    Serial.println(" °C");
  } else {
    Serial.println("DHT22 checksum failed");
  }

  delay(3000);  // Minimum delay between reads
}


