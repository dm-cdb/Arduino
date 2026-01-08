To manage a pin with an Arduino, we would most often use the well known function :<br>
- pinMode(pin, OUTPUT|INPUT|INPUT_PULLUP);
- digitalWrite(pin, 0|1);
- digitalRead(pin); Read the state of a pin in INPUT mode (return 0 if LOW or 1 if HIGH )

INPUT_PULLUP will connect the PIN (in INPUT mode) to an internal "pull up resistor" ; that is a resistor (between 20K Ohms and 50K Ohms - around 40K Ohms on one of my Arduino Nano) connected to VCC (5V here).<br>
The PULL UP resistor will ensure the PIN is in a known state (HIGH), and not "floating", unless the pin is connected to ground.

![Pin-pull-up](https://github.com/user-attachments/assets/6b77059a-ac19-4ff4-aa4d-36667e929e88)

These Arduino functions actually induce a lot of delays if you want to toggle a led. For example a digitalWrite() can take up to 10µs.

Another and quicker way is to access the GPIO registers directly, using bitwise operation.<br>
There are three registers to consider : DDRn, PORTn, and PINn<br>
Each registers are 8 bits, so they can handle 8 ports.<br>
On an Arduino Nano, use the following ports registers / pins map :

PD = D0, D1, D2, D3, D4, D5, D6, D7 <br>
PB = D8, D9, D10, D11, D12 et D13 <br>
PC = D14, D15, D16, D17, D18, D19 <br>

Note : the "D" in D0, D1 etc. here stands for "Digital", as opposed to "Analog".

So PD controls the first 8 pins, that is D0 to D7 (knowing that D0 is also used for serial Rx, and D1 for serial Tx - avoid these pins for playing with leds or what not)<br>

To control the GPIO pins, use the following registers :<br>
- DDRn will set the INPUT/OUTPUT mode of a pin : 0 is input, 1 is output. Below the register for DDRD (controls pins D0 to D7) :

![arduino-ddrd](https://github.com/user-attachments/assets/b1d63d68-c92a-45e7-b378-eb1714867b40)

We can see that by default all pins are in input mode (0).<br>

-PORTn will set a pin in OUTPUT mode to LOW (0) of HIGH (1). Below the register for PORTD (controls pins D0 to D7)  :

![arduino-portd](https://github.com/user-attachments/assets/7e51279b-d930-4802-a04b-9de6dd5ecf04)

We can see that by default all pins are in LOW mode (0).<br>

-PINn will read the state of a pin in INPUT mode : HIGH (1) or LOW (0). Below the register for PIND  (controls pins D0 to D7)   :

![arduino-pind](https://github.com/user-attachments/assets/c0a87e39-3040-4581-940d-250b5cc9f855)

- Note that this register is read-only.

In the following example we will use pin D5 :

To set (or not) the PULLUP resistor on pin D5 : 
- DDRD bit 5 must remains at 0 (input mode).
- PORTD bit 5 must be set to 1 (activate PULLUP) ; by default  set to 0, no PULLUP

  To read the state of pin D5 in input mode, use PIND register :
  - If bit 5 reads 1, the input is HIGH (at VCC 5V, input must be above 3V)
  - If bit 5 reads 0, the input is LOW (at VCC 5V, input must be below 1,5V)

  (Note : there is a register that disable all PULLUP resistors - bit 4 i MCUCR register -  ; it will be ignored here)

To set pin D5 in output mode :
- DDRD bit 5 must be set to 1 (onput mode).
- PORTD bit 5 can then be set to 1 HIGH (5V, source up to 40 mA) ; or set to 0 LOW (ground, can sink up to 40mA).

  Below is example to put D5 in output mode ; it can be done differently :<br>
  - DDRD |= B00100000; // Positionnement de D5 en OUTPUT
  - DDRD |= (1<<5);
  - DDRD |= (1<<PD5);

  



