This is a very simple code to read the humidity and temperature data from an guenine ASAIR/AOSONG DHT11 sensor.
Please note that a guenine ASAIR DHT11 Temperature and Humidity Sensor Module support tempature range from -20°C to + 60°C, unlike cheap copy that support only 0°c to + 50°C.
Offical web site : https://www.aosong.com/en/Products/info.aspx?itemid=2257&lcid=139

The code suppport the negative temperature, is made specificaly for the DHT11, but is very simple to adapt to other AOSONG products like the DHT22/AM2302.
No need for external libraries. Should run asis direcly.
Please be careful : some DHT11 are sold with a pull up resistor included ; for "raw" DHT11, place a pull up resistor as specfied in the datasheet between PIN2 and VCC (5k - 10k is OK).
