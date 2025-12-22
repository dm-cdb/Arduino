/* This is an adapted code to use the 32 bits integers compensation formula from Bosch API.
 * This code is hence more adapted to ATMega328P microcontroller with limited resources.
 * See github.com/boschsensortec/BME280_SensorAPI/blob/master/bme280.c
 * Christian de Balorre,  Nov 20, 2024
*/

#define FOSC 16000000UL  // Clock frequency = Oscillator freq .
#define BDIV ((FOSC / 100000) - 16) / 2

// BME280 default 7bits address is 0x76 - 0b1110110
#define BME280_ADDRESS 0xEC  // Default = 111011x : SDO ground : x = 0 | Vddio x = 1
#define BME280_ID 0xD0
#define BME280_CONFIG 0xF5
#define BME280_CMEAS 0xF4
#define BME280_CHUM 0xF2  // Change to this register effective after wr to CTRL_MEAS
#define BME280_PREG 0xF7  // Pa register (0xF7 -> 0xF9)
#define BME280_TREG 0xFA  // T° register (0xFA -> 0xFC)
#define BME280_HREG 0xFD  // RH register (0xFD -> 0xFE)
#define BME280_TCAL 0x88
#define BME280_H1CAL 0xA1
#define BME280_HCAL 0xE1

// Change these values to your needs
#define H_MODE 0x02     // osrs_h * 2 : 0x00 = disable humidity sampling
#define TPF_MODE 0x4B   // osrs_t * 2 , osrs_p * 2, mode : normal
#define CONF 0x64       // Standbye 250ms, filter coeff 2

bool ack = 0;
bool nack = 1;
long t_fine;

struct data {
  byte msb;
  byte lsb;
  byte xlsb;
};

struct t_calib {
  unsigned int T1;
  int T2;
  int T3;
};

struct p_calib {
  unsigned int P1;
  int P2;
  int P3;
  int P4;
  int P5;
  int P6;
  int P7;
  int P8;
  int P9;
};

struct h_calib {
  byte H1;
  int H2;
  byte H3;
  int H4;
  int H5;
  char H6;
};

// Global vars
struct t_calib dig_t;
struct h_calib dig_h;
struct p_calib dig_p;

// Basic I2c functions
// Set clock rate and enable i2c controler and SDA/SDC ports
// ATmega328P : SCL = D19/PC5/A5 ; SDA = D18/PC4/A4
void i2c_init(void) {
  pinMode(18, INPUT);
  pinMode(19, INPUT);
  TWBR = BDIV;  // Bit rate : 100kHz@16MHz = 72 for prescaler = 1
  TWSR = 0x00;  // Status   : prescaler = 1
  TWCR = 0x00;
  bitSet(TWCR, TWEN);  // Enable i2c bloc and ports

  return;
}

void i2c_bye(void) {
  bitClear(TWCR, TWEN);  // Disable i2c bloc and SDA/SDC ports

  return;
}
// Start Master read/write session
// Note : TWSTA bit NOT cleared by hardware
byte i2c_start(void) {
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
  while ((TWCR & 0x80) == 0);  // Wait for int flag to be set

  return (TWSR & 0xF8);  // Should be 0x08
}

void i2c_stop(void) {
  TWCR = _BV(TWINT) | _BV(TWSTO);

  return;
}

// Send a data byte
byte i2c_wrByte(byte data) { 
  byte i = 0;
  TWDR = data;
  // Clear the int flag, send byte
  TWCR = _BV(TWINT) | _BV(TWEN);
  while ((TWCR & 0x80) == 0) { // Wait for twint flag to be set : < 100µs @ 100kbps
    delayMicroseconds(3);
    i++;
    if(i > 200) {             // Timeout 600µs
      Serial.print("Device timeout");
      break;
    }
  }
  return (TWSR & 0xF8);
}

// Receive a data byte
// If stop is set, will send a nack
byte i2c_rdByte(bool stop) {
  byte i = 0;
  if (stop) {
    TWCR = _BV(TWINT) | _BV(TWEN);  // Send NACK (TWEA =0) if last byte transmitted
  } else {
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN);  // Send regular ACK
  }
  while ((TWCR & 0x80) == 0){ // Wait for twint flag to be set  : < 100µs @ 100kbpbs
    delayMicroseconds(3);
    i++;
    if(i > 200){              // Timeout 600µs
      Serial.print("Device timeout");
      return (TWSR & 0xF8);
    }
  }
  return (TWDR);
}

void bme280_init() {
  byte stat;
  stat = i2c_start();
  if (stat != 0x08) {
    Serial.print("IO error - check SCL/SDA pins ");
    Serial.print(stat, HEX);
  }
  i2c_wrByte(BME280_ADDRESS & 0xFE);
  i2c_wrByte(BME280_ID);  // Request device ID register
  i2c_start();
  i2c_wrByte(BME280_ADDRESS | 0x01);
  stat = i2c_rdByte(nack);
  i2c_stop();

  if (stat != 0x60) {
    Serial.print("Unknown device : 0x");
    Serial.println(stat, HEX);
  } else {
    Serial.println("BME280 online");
  }
  // Prepare sensor to T°, Pa, RH reading ; note : when sensor disabled, output = 0x800000
  i2c_start();
  i2c_wrByte(BME280_ADDRESS & 0xFE);
  i2c_wrByte(BME280_CONFIG);
  i2c_wrByte(CONF);  // Standby 250ms, filter 2, spi off
  i2c_stop();
  delay(5);
  i2c_start();
  i2c_wrByte(BME280_ADDRESS & 0xFE);
  i2c_wrByte(BME280_CHUM);
  i2c_wrByte(H_MODE);
  i2c_stop();
  delay(5);
  i2c_start();
  i2c_wrByte(BME280_ADDRESS & 0xFE);
  i2c_wrByte(BME280_CMEAS);
  i2c_wrByte(TPF_MODE);
  i2c_stop();

  Serial.println("Reading compensation data...");
  // Read and populate T° & P compensation struct
  bme280_tpdig();
  // Read and populate RH compensation struct
  bme280_hdig();

  delay(100);

  return;
}

byte bme280_check(void) {
  byte ret;
  i2c_start();
  ret = i2c_wrByte(BME280_ADDRESS & 0xFE);
  i2c_stop();

  return ret;
}

long bme280_getdata(byte dev_addr, byte reg_addr) {
  struct data myData;
  long ret;
  i2c_start();
  i2c_wrByte(dev_addr & 0xFE);
  i2c_wrByte(reg_addr);
  i2c_start();
  i2c_wrByte(dev_addr | 0x01);
  myData.msb = i2c_rdByte(ack);
  myData.lsb = i2c_rdByte(ack);
  myData.xlsb = i2c_rdByte(nack);
  i2c_stop();

  ret = ((int)(myData.msb)) << 8;
  ret = ret | myData.lsb;
  if (reg_addr == 0xFD) {  // Humidity is 16 bits only (no xlsb)
    ret = ret & 0x0000FFFF;
    return ret;
  }
  ret = ret << 4;  // Pressure and temperature are 20 bits
  myData.xlsb = myData.xlsb >> 4;
  ret = ret | myData.xlsb;
  ret = ret & 0x000FFFFF;

  return ret;
}

// Read and populate RH compensation struct
void bme280_hdig(void) {
  byte digl;
  byte digh;
  i2c_start();
  i2c_wrByte(BME280_ADDRESS & 0xFE);
  i2c_wrByte(BME280_H1CAL);
  i2c_start();
  i2c_wrByte(BME280_ADDRESS | 0x01);
  dig_h.H1 = i2c_rdByte(nack);
  i2c_start();
  i2c_wrByte(BME280_ADDRESS & 0xFE);
  i2c_wrByte(BME280_HCAL);
  i2c_start();
  i2c_wrByte(BME280_ADDRESS | 0x01);
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_h.H2 = ((int)digh << 8) | digl;
  dig_h.H3 = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);  // 0xE4
  digl = i2c_rdByte(ack);  // 0xE5
  // H4 is 12 bits from 0xE4 and Lnibble of 0xE5
  dig_h.H4 = ((int)digh << 4) | (digl & 0x0F);
  digh = i2c_rdByte(ack);  // 0xE6
  digl = digl & 0xF0;
  digl = digl >> 4;
  // H5 12 bits from E6 and hnibble of 0xE5
  dig_h.H5 = ((int)digh << 4) | digl;
  dig_h.H6 = i2c_rdByte(nack);
  i2c_stop();

  return;
}

// Read and populate T° & P compensation struct
void bme280_tpdig(void) {
  byte digl;
  byte digh;
  i2c_start();
  i2c_wrByte(BME280_ADDRESS & 0xFE);  // Master write
  i2c_wrByte(BME280_TCAL);
  i2c_start();  // Restart & Master read
  i2c_wrByte(BME280_ADDRESS | 0x01);
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_t.T1 = ((int)digh << 8) | digl;  // Should be 0x6E90
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_t.T2 = ((int)digh << 8) | digl;  // Should be 0x68ED
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_t.T3 = ((int)digh << 8) | digl;  // Should be 0x0032
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_p.P1 = ((int)digh << 8) | digl;
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_p.P2 = ((int)digh << 8) | digl;
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_p.P3 = ((int)digh << 8) | digl;
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_p.P4 = ((int)digh << 8) | digl;
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_p.P5 = ((int)digh << 8) | digl;
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_p.P6 = ((int)digh << 8) | digl;
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_p.P7 = ((int)digh << 8) | digl;
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(ack);
  dig_p.P8 = ((int)digh << 8) | digl;
  digl = i2c_rdByte(ack);
  digh = i2c_rdByte(nack);
  dig_p.P9 = ((int)digh << 8) | digl;
  i2c_stop();

  return;
}
// See github.com/boschsensortec/BME280_SensorAPI/blob/master/bme280.c for 32 bits compensation formulas
void bme280_dotemp(long *res) {
  long var1, var2;
  *res = 0;
  *res = bme280_getdata(BME280_ADDRESS, BME280_TREG);

  var1 = (*res >> 3) - ((long)dig_t.T1 << 1);
  var1 = (var1 * dig_t.T2) >> 11;
  var2 = (*res >> 4) - dig_t.T1;
  var2 = (((var2 * var2) >> 12) * dig_t.T3) >> 14;
  t_fine = var1 + var2;
  *res = ((t_fine * 5) + 128) >> 8;

  return;
}

void bme280_dorh(long *res) {
  long var1, var2, var3, var4, var5;
  *res = 0;
  *res = bme280_getdata(BME280_ADDRESS, BME280_HREG);

  var1 = t_fine - 76800;
  var2 = *res << 14;
  var3 = (long)(dig_h.H4) << 20;
  var4 = (long)(dig_h.H5) * var1;
  var5 = (((var2 - var3) - var4) + 16384) >> 15;
  var2 = (var1 * (long)(dig_h.H6)) >> 10;
  var3 = (var1 * (long)(dig_h.H3)) >> 11;
  var4 = ((var2 * (var3 + 32768)) >> 10) + 2097152;
  var2 = ((var4 * (long)(dig_h.H2)) + 8192) >> 14;
  var3 = var5 * var2;
  var4 = ((var3 >> 15) * (var3 >> 15)) >> 7;
  var5 = var3 - ((var4 * ((long)dig_h.H1)) >> 4);
  var5 = (var5 < 0 ? 0 : var5);
  var5 = (var5 > 419430400 ? 419430400 : var5);
  *res = var5 >> 12;

  return;
}

void bme280_dopressure(long *res) {
  long var1, var2, var3, var4, var5, press_temp;
  *res = 0;
  *res = bme280_getdata(BME280_ADDRESS, BME280_PREG);

  var1 = (t_fine >> 1) - 64000;
  var2 = ((var1 >> 2) * (var1 >> 2) >> 11) * dig_p.P6;
  var2 = var2 + ((var1 * dig_p.P5) << 1);
  var2 = (var2 >> 2) + ((long)(dig_p.P4) << 16);
  var3 = (dig_p.P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3;
  var4 = (dig_p.P2 * var1) >> 1;
  var1 = (var3 + var4) >> 18;
  var1 = ((32768 + var1) * dig_p.P1) >> 15;

  // Avoid exception caused by division by zero : simplified with no max nor min - diff in comments
  if (var1) {
    var5 = 1048576 - *res;
    press_temp = (var5 - (var2 >> 12)) * 3125;
    if (press_temp < -2147483648) {  // Force signed comparison
      press_temp = (press_temp << 1) / var1;
    } else {
      press_temp = (press_temp / var1) << 1;
    }
    var1 = 4285 * ((press_temp >> 3) * (press_temp >> 3) >> 13);  // Avoid 64 bits data overflow...
    var1 = var1 >> 12;
    var2 = ((press_temp >> 2) * dig_p.P8) >> 13;
    press_temp = press_temp + ((var1 + var2 + dig_p.P7) >> 4);
    *res = press_temp;
  } else {
    *res = 30000;
  }

  return;
}

void setup() {
  Serial.begin(9600);
  i2c_init();
  Serial.println("Initializing device");
  bme280_init();
  Serial.println("T °C\tRH %\t Pr hPa");
}


void loop() {
  long res;
  unsigned int dec, intg;
  byte sign = '+';
  byte stat = 0;
  bool err = 0;

  // BME 280 sanity check
  do {
    stat = bme280_check();
    if (stat != 0x18) {
      Serial.println("Device not responding...");
      Serial.print("Error 0x");
      Serial.println(stat, HEX);
      err = true;
      stat = 1;
    }
    else {
      stat = 0;
      if (err) {
        Serial.println("Device reinit...");
        err = false;
        i2c_init();
        bme280_init();
        break;
    }
    
    }
    delay(2000);
  } while(stat);

  // Use float variable for easier & faster coding
  // Use int variables for 8bits µc resource sparing

  bme280_dotemp(&res);
  // res = int32 in 0.01°C ie 5123 = 51.23°C
 
  if (res >> 31) {  // Test bit sign ; run 2 bits complement if negative)
    res = ~res + 1;
    sign = '-';
  }
  intg = (unsigned int)(res / 100);
  dec = (unsigned int)(res % 100);

  Serial.write(sign);
  Serial.print(intg, DEC);
  Serial.write(0x2e);
  if (dec < 0x0A) Serial.write(0x30);
  Serial.print(dec, DEC);
  Serial.print('\t');

  bme280_dorh(&res);
  // res = uin32t in Q22.10 int.frac format
  dec = (int)(res & 0x000003FF);
  dec = dec / 10;  // 2 decimal precision more than enough
  intg = (int)(res >> 10);

  Serial.print(intg, DEC);
  Serial.write(0x2e);
  if (dec < 0x0A) Serial.write(0x30);
  Serial.print(dec, DEC);
  Serial.print('\t');


  bme280_dopressure(&res);
  // res = uin32t Pa
  intg = (int)(res / 100);
  dec = (int)(res % 100);

  Serial.print(intg, DEC);
  Serial.write(0x2e);
  if (dec < 0x0A) Serial.write(0x30);
  Serial.print(dec, DEC);
  Serial.println();
  delay(3000);
}


