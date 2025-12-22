** Important **<br>
- The code is based on the ASAIR documentation Version Number:V1.3_20170331<br>
- This documentation states that ONLY the decimal byte of Humidity is always zero.<br>
- Other documentations claim that the decimal Temperature byte is also always zero. It appears to be not true for guenine ASAIR DHT11 modules.

Here is how the data stream is sent (MSB first) :

|Bytes  | Data               |
|-------|--------------------|
Byte 1  | Humidity integer   |
Byte 2  | Humidity decimal   |
Byte 3  | Temperature integer|
Byte 4  | Temperature decimal|
Byte 5  | Parity check       |

The present Arduino code is very simple for reading the humidity and temperature data from an guenine ASAIR/AOSONG DHT11 sensor.<br>
It should also be able to read clones as well : what can do the more can do the less.

Please note that a guenine ASAIR DHT11 Temperature and Humidity Sensor Module supports :<br>
- Tempature range from -20°C to + 60°C (unlike clones that support only 0°C to + 50°C).<br>
- Humidity range from 5% to 95% (unlike clones that support only 20% to 95%).<br>

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

You get the picture... So there is no way to distinguish between a -9,0°C and a -10,0°C for exemple - as far as I know. This is a hardware bug ; up to now I cannot imagine a clear solution.

The negative temperature bit is set in the MSB bit of the temperature decimal byte (since the precision is just one decimal, only the last 4 bits will be used). When set, the sensor outputs a negative temperature. <br>
The code just test the presence of this bit, then clears it to compute the decimal part.

Please be careful : DHT11 bundled with a breakout board are usually sold with the pull up resistor included ; for "raw" DHT11, place a pull up resistor as specfied in the datasheet between PIN2 and VCC (5k - 10k is OK).<br>
Most of the bundled DHT11 are actually clones with limited specs.

![dht11-10k pullup](https://github.com/user-attachments/assets/0ce69a6a-042e-4991-ab59-46d307a2e466)

Below is a schematic from the official ASAIR datasheet :

![DHT11-schematic](https://github.com/user-attachments/assets/f28a81bd-ee73-4d07-b700-66bcf61f6179)

