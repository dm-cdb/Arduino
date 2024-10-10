There is a lot of confusion regarding the inernal temperature sensor of the AVR ATmega 8bits device, most notably the ATmega328p used in the Arduino Nano for example.
This is a small synthetisis of three AVR documents regarding this feature : <br>
<a href="https://ww1.microchip.com/downloads/en/AppNotes/Atmel-8108-Calibration-of-the-AVRs-Internal-Temperature-Reference_ApplicationNote_AVR122.pdf">AVR122: Calibration of the AVR's Internal Temperature Reference</a><br>
<a href="https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf">ATmega328P datasheet, 2015</a><br>
<a href="https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf">megaAVR datasheet, 2018</a><br>

Here is a basic description of the captor technology used in these devices, from AVR122 :<br>
"The temperature measurement is based on an on-chip temperature sensor that is coupled to a single ended ADC channel [actually ADC8 - see schematic below]. The sensor is a diode that produces a temperature dependent voltage. The voltage has a linear relationship to temperature and the result has approximately a 1 LSB/°C correlation to temperature ... The diode voltage is highly linear, but due to process variations the temperature sensor output voltage varies from one chip to another."<br>
<br>
This basically tells us that the sensor will measure not the ambiant air temperature, but the temperature/heat of the internal silicon die dissipated at the junction with the chip case. As the internal silicon die temperature will vary with the current/load used by the chip in relation with the activation of its pins, it means this internal sensor will be closed to the ambiant air only while practically idle, at least with no active output pins sourcing current. 
<br>
The last line simply tells us that due to the manufactoring process, each device needs some kind of indivudual calibration to reflect the real temperature of the device. <br>
Below is the ADC topologu schematic used in the ATmega328P :<br>
<br>
![Atmega328P-adc-schematic](https://github.com/user-attachments/assets/8b8b184a-34a8-4d6d-be8f-38e35d60fb3a)
<br>
The sensor is again really a simple diode, which voltage drop varies linearly according to the temperature. The ADC chanel reads the actual voltage across the diode, and output the converted value.<br>
Now the AVR122 note explains that the ADC output is equal to the temperature in Kelvin (K). So T = 0°C should ideally be equal to ADC = 273 ; and ADC = 0 would be equal to absolute zero, -273°C... This is in theory ; for the reasons already given, it is never the case, and we need to adjust or compensate the ADC result.<br>

The datasheets also say : "The voltage sensitivity is approximately 1 LSB/°C". This means that each unit of the ADC value represent a change of 1°. So a change of 0.001 volt (1mV) measured acrosse the diode at ADC8 implies a change of 1° in the temperature.<br>
But, due to not so perfect manufacturing process, the datasheet warns : "However, due to the process variation the temperature sensor output varies from one chip to another." We then need to bring some corrections to the raw ADC value.<br>
To sum up :<br>
- The ideal line should be strictly linear, in the form : y = x, where 0° read from ADC (ie 273) = 0° in the real life (IRL).<br>
- The actual line would instead be an affine line, in the form : y = ax + b (a being the coefficient or slope, and b a constant).<br>
<br>
In the ideal world, ADC(0°C) = 273 = IRL(0°C)
We read further in AN122 : "The offset error is the distance between the origin of coordinates and the point where the line defined by the two calibration points intersects the y-axis."<br>
Put it simply, it means that instead of reading y = x, with y = ADC value and X real life temperature, ie ADC(0°C) = IRL(0°C), we'll have y = x + b. For exemple, instead of reading ADC(0°) = 273 for IRL(0°), we could have ADC(0°) = 280. It means we have an offset of +8 relative to IRL(0°). <br>
So to have a correct reading of the ADC value, we'll need to have y - 8 = x : or y = x + 8. This would be the OFFSET or TS_OFFSET in the datasheet.<br>
See diagram below : <br>
<br>
![temp-offset](https://github.com/user-attachments/assets/1e032c5f-2e1d-43bd-9e2c-236657029c4f)
<br>
The second correction is called gain factor (or TS_GAIN in datsheet), and is a result of the slope / coefficient of the line not being strictly = 1, because of the 1.1V reference voltage slightly varying for example.<br>
<br>
![temp-gain](https://github.com/user-attachments/assets/3d4f7c53-6190-47c7-8eee-6f37fdb4c80d)
<br>
Basically we are trying to find the slope or coeffient "a" in the equation y = ax + b.
<br>
One other interesting thing to note is the difference in datasheets 2015 & 2018 regarding the presentation of the internal temperature sensor (Analog to digital conversion / temperature measurement section).<br>
2018 edition says for typical values : <br>
  Table 24-2. Temperature vs. Sensor Output Voltage (Typical Case) <br>
Temperature / °C   -45°C   +25°C   +85°C <br>
Voltage / mV       242mV   314mV   380mV <br>
(Respectively ADC value of 242, 314, 380)<br>
<br>
2015 edition : <br>
Table 23-2. Sensor Output Code versus Temperature (Typical Values) <br>
Temperature/°C     –40°C   +25°C   +125°C <br>
ADC raw value      0x010D   0x0160   0x01E0 <br>
(Respectively ADC/mV value of 269, 352, 480)<br>
<br>
In three years interval, quite some discreapancy in the typical values ! So unfortunately, we cannot rely on these typical values to calulate a reasonable output.<br>
<br>
Also we note two different approches to make for compensation: <br>
2015 datasheet : ((ADCW – (273 + 100 – TS_OFFSET)) * 128) / TS_GAIN + 25 ; with ADCW the result of register (ADCH<<8) + ADCL. The result is in °C. <br>
2018 datasheet : (T = ADCW - Tos) / k ; "Tos is the temperature sensor offset value determined and stored into EEPROM as a part of the production test. k is a fixed coefficient".<br>
<br>
We don't have further explainations in the 2018 datasheet, except that a calibration must be done, and the result stored in EEPROM. We can only assume that T = temperature in Kelvin.<br>
The 2015 datasheet explains that TS_OFFSET and TS_GAIN can be found in the Signature Bytes respectively at address 0x0002 and 0x0003. However actually no useful data can be retrieved from these addresses. <br>
The following explains how to simply calibrate a device.
<br>

<u>Setup :</u><br>
From the schematic, it can be seen that a temperature sensor is attached directly to a dedicated ADC channel. That would be ADC8, however note it cannot be accessed directly with a digitalRead from the Arduino IDE.<br>
Details of how to set up the configuration to read the sensor can be obtained from the 2015 Atmega328P datasheet p. 214.<br>
  - Select the 1.1V internal reference volatge. This is done by setting bits REFS1 and RFS0 of ADMUX register to 1 respectively (there is no need to set any pin to input mode, since this is a dedicated channel). <br>
  - Select channel 8 by setting bit of MUX3 of ADMUX register.
  - Next set the prescaler for the correct conversion clocking. It requires an input clock frenquency between 50kHz and 200kHz for maximum resolution (10 bits).<br>
  - For a 16Mhz Arduino Nano, a prescaler of 128 will yield a 125kHz clock. Set then ADPS2 to ADPS0 bits of ADCSRA register to 1.<br>
  <br>

The formula given is : 
  
![adc-temp-formula](https://github.com/user-attachments/assets/36a52585-5e9e-45a9-a5bf-52f3e3751464)

Now the problem is that the correction factor TS_OFFSET and TS_GAIN are usually nowhere to be seen in the device signature octets, as advertised by the datasheet. We propose here is a way around to get reasonable data.<br>
First note that the formula add 25. So for 25°C measured, it means that the dividend of the formula must be equal zero. All we have to do is then to obtain the ADC value when the case temperature is 25° (measured with a digital thermometer)<br>
Once you have a reasonable value for 25°, you can calculate TS_OFFSET with :<br>
TS_OFFSET = ADCW - (273 + 100), with ADCW = (ADCH<<8) + ADCL). For my device at 25° the raw value was 317 (ie 0.317v measured by the conversion), which gives an offset of 56.<br>
To get TS_GAIN will be more complicated. A crude way is put the device directly into a freezer for a while : it can support Celsius t° between -40° and +80° according to the Arduino Nano datasheet, but do it at your own risk anyway :-).<br>
Just program the device before so that it can send the raw ADC value of the temperature sensor on the serial monitor.<br>
Remove the device from freezer, connect it quickly to the PC/serial line and take note of the ADC value plus that of a digital thermometer applied to the device case. For exemple measured at 5°, my device yielded a raw value of 294.
You can also put the device close to a lamp so it can heat up to relatively high temperature (beware burning etc.) Do not go other 50°C.<br>
The idea here is to go as extreme as possible, so that the impact of the ambiant temperature far surpass that of the internal working temperature in normal condition.<br>
Once you have some raw values for given temperatures, there will be only one unknown y (for gain) left in the equation, namely :
[(raw value - (373 - offset)) * 128] / y + 25 (note : we will iggnore the * 128 coefficient for now) <br>
For 5° = 294 it gives : <br>
(294 - (373 - 56)) / y + 25 = 5 <-> y = -2944 / -20 = 147.2. We then need to divide this value by 128 : "TS_GAIN is the unsigned fixed point 8-bit temperature sensor gain factor in
1/128th units."<br>
TS_GAIN = 147.2 / 128 = 1.15 in my case.<br>
<br>
The complete formula for my device would then be :<br>
(ADCW - 317) / 1.15 + 25.





