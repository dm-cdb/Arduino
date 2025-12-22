This is a very simple code to read the humidity and temperature data from an guenine ASAIR/AOSONG DHT11 sensor.<br>
Please note that a guenine ASAIR DHT11 Temperature and Humidity Sensor Module support tempature range from -20°C to + 60°C, unlike cheap copy that support only 0°c to + 50°C.<br>
Offical web site : https://www.aosong.com/en/Products/info.aspx?itemid=2257&lcid=139

The code suppport the negative temperature of the device, with one bug though. It does not increment the integer part of temperature, when decimal part goes from .9 and "up".
Here is an exemple (the DHT was put in a freezer :

```
Humidity: 15 %  Temperature: -1,8 °C
Humidity: 16 %  Temperature: -1,0 °C <--------- should be -2°C
Humidity: 16 %  Temperature: -2,2 °C
...
Humidity: 17 %  Temperature: -9,8 °C
Humidity: 17 %  Temperature: -9,9 °C
Humidity: 17 %  Temperature: -9,0 °C <---------- should be -10°C
Humidity: 17 %  Temperature: -10,1 °C
'''
Humidity: 17 %  Temperature: -10,9 °C
Humidity: 18 %  Temperature: -10,0 °C <---------- should be -11°C
Humidity: 18 %  Temperature: -11,1 °C
```

You get the picture... So there is no mean to distinguish between a -4,0C° and a -5,0°C - as far as I know.<br>
The code is made specificaly for the DHT11, but is very simple to adapt to other AOSONG products like the DHT22/AM2302.<br>
No need for external libraries.<br>
Please be careful : some DHT11 are sold with a pull up resistor included ; for "raw" DHT11, place a pull up resistor as specfied in the datasheet between PIN2 and VCC (5k - 10k is OK).
