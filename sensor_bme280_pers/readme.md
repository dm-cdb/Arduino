This is a lightweight Arduino program to read a Bosch SensorTec BME280 sensor using i2c protocol.<br>
The BME280E is a highly accurate sensor that provide measurements of temperature, pressure and humidity.<br>
There exists quite a few libraries for this sensor, one the best being at <a href="https://github.com/hasenradball/Bosch_BME280_Arduino/tree/master">hasenradball github</a>, that directly includes Bosch Sensortec API.<br>
However the somewhat complex compensation formulas put quite a heavy stress on 8 bits µcontroller resources like the ATMEga328p in Arduino, due to the frequent use of signed long and float variables.<br>
Actually this sensor is more recommended for controller like the RP2040 on the Raspberry PI Pico for example ; nonetheless decent accuracy can be obtained on 8bits µcontrollers.<br>
<br>
This program is a lightweight version that uses Bosch 32bits integers formulas to compute the different measurement, to be found on <a href="https://github.com/boschsensortec/BME280_SensorAPI">Bosch Sensortec github<a/>.<br>
You can compare the results with the more advanced version of hasenradball if you like.<br>
No float variables are used here, so the measurement reading are little more complicated ; but if you wish you can of course simplify and rewrite the code at your will.<br>
Also it uses its own i2c primitives - again lighter code, but maybe not completely sanity proof.
