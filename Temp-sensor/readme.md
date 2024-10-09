This code takes advantage of the internal temperature sensor of the ATmega328P, found for example on the Arduino Nano.<br>
The sensor is really simply a diode, which voltage drop varies lineary according to the temperature. The ADC channel simply reads the actual voltage across the diode, and a function transforms the value into temperature.
The ATmega328P datasheet specifies the conditions to correctly interpret the value.<br>

![Atmega328P-adc-schematic](https://github.com/user-attachments/assets/8b8b184a-34a8-4d6d-be8f-38e35d60fb3a)

It should be noted however that the sensor does not measure the ambiant air directly, but the working temperature of the diode inside the chip case.<br>
Of course this temperature is also dependant on ambiant temperature, but will mostly be affected in normal condition by the power dissipated by the chip itself and the current load it must handle.<br>
It will nonetheless give a rough idea of the ambiant temperature when pretty much idle (no pin used)...<br>
<br>
<u>Setup :</u><br>
From the schematic, it can be seen that a temperature sensor is attached directly to a dedicated ADC channel. That would be ADC8, but note it cannot be accessed directly with a digitalRead from the Arduino IDE.<br>
Details of how to set up the configuration to read the sensor can be obtained from the 2015 Atmega328P datasheet p. 214.<br>
  - Select the 1.1V internal reference volatge. This is done by setting bits REFS1 and RFS0 of ADMUX register to 1 respectively (there is no need to set any pin to input mode, since this is a dedicated channel). <br>
  - Select channel 8 by setting bit of MUX3 of ADMUX register.
  - Next set the prescaler for the correct conversion clocking. It requires an input clock frenquency between 50kHz and 200kHz for maximum resolution (10 bits).<br>
  - For a 16Mhz Arduino Nano, a prescaler of 128 will yield a 125kHz clock. Set then ADPS2 to ADPS0 bits of ADCSRA register to 1.<br>
  <br>
With a 1.1 reference voltage, the maximum resolution with a 10bits ADC will be 1.1 / 1024 = 0.001 ; this means that each unit (or LSB) in the result of the ADC conversion represents 0.001 volt.<br>
So if ADCW = (ADCH<<8) + ADCL = 100, the actual voltage measured by the ADC is 0,1V.<BR>
<br>
The datasheet says : "The voltage sensitivity is approximately 1 LSB/°C". It means that each unit of the ADC value represent a change of 1°. So a change of 0.001 volt (1mV) measured implies a change of 1° in temperature.<br>
But, due to not so perfect manufacturing process, the datasheet warns : "However, due to the process variation the temperature sensor output varies from one chip to another."<br>
We then need to bring some corrections to the raw ADC value.
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





