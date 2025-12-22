This is a very simple code to read the humidity and temperature data from an guenine ASAIR/AOSONG DHT11 sensor.<br>
Please note that a guenine ASAIR DHT11 Temperature and Humidity Sensor Module supports :<br>
- Tempature range from -20°C to + 60°C (unlike cheap clones that support only 0°C to + 50°C).<br>
- Humidity range from 5% to 95% (unlike cheap clones that support only 20% to 95%).<br>

Offical web site : https://www.aosong.com/en/Products/info.aspx?itemid=2257&lcid=139

![ASAIR-original](https://github.com/user-attachments/assets/fb059bb1-d049-4c6b-8b39-96120a4dfffb)

The code suppport the negative temperature of the device, with one bug though. It does not increment the integer part of temperature, when decimal part goes from .9 and "down" to the next negative integer.
Below is an exemple (the DHT was put in a freezer) :

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

You get the picture... So there is no way to distinguish between a -4,0C° and a -5,0°C - as far as I know.<br>
The code is made specificaly for the DHT11, but is very simple to adapt to other AOSONG products like the DHT22/AM2302.<br>
No need for external libraries.<br>
Please be careful : some DHT11 are sold with a pull up resistor included ; for "raw" DHT11, place a pull up resistor as specfied in the datasheet between PIN2 and VCC (5k - 10k is OK).

![dht11-10k pullup](https://github.com/user-attachments/assets/0ce69a6a-042e-4991-ab59-46d307a2e466)

Below is a schematic from the official ASAIR datasheet :

![DHT11-schematic](https://github.com/user-attachments/assets/f28a81bd-ee73-4d07-b700-66bcf61f6179)

