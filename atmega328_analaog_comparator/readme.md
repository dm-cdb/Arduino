<h3 align="center">Arduino Nano Analog Comparator</h3>

Here is a simple yet not very well kown feature you can find on an Arduino with an ATMega328P : an analog comparator.<br>
Below is a very basic schematic of the comparator :

![ATMega328-comp](https://github.com/user-attachments/assets/6115222f-0804-44af-a4b2-11f88c114688)

- AIN0 is called the non-inverting input (+ pin of the comparator)<br>
- AIN1 is called the inverting input (- pin of the comparator)<br>
- AIN0 is located on pin D6
- AIN1 is located on pin D7 (playing with different registers ADCSRB and ADCSRA, you have the option to select any ADC pin as AIN1 - check ATMega328P datasheet 22.2 <br>

When AIN0 voltage > AIN1 voltage, the ACO bit of ACSR register is set ;<br>
When AIN1 voltage > AIN0 voltagen the ACO bit of ACSR register is cleared. <br>

![analog-comp-register](https://github.com/user-attachments/assets/3057645f-30be-492f-8113-475d963f28ce)


!! Important AIN0 or AIN1 input voltage CANNOT be > Vcc that is 5V for an Arduino Nano !!

AIN0 can be :
- An external voltage.
- Or an internal voltage reference if you set the ACBG bit (At VCC = 5V, between 1V and 1,2V).

Also we will modify this register : <br>

![IDR1-register](https://github.com/user-attachments/assets/32a18e04-6842-442c-b584-497fbb5bfbd1)

When AIN1D and/or AIN0D is set, the digital input buffer on the AIN1/0 pin is disabled.<br>
This basically disable the digial input circuitry of pins D6 and D7.<br>

This project uses the basic feature of the comparator ; there are much more advanced options using interrupts and Timer1 in capture mode.<br>

Below is a schematic of this small project made with SimulIDE : when AIN1 > AIN0, the green led is ON. When not, the red led is ON.<br>
Can be used to monitor a battery for ex.<br>


![nano-comp-simulide](https://github.com/user-attachments/assets/9f905db4-f0d9-442e-8802-0d9f6dc0082a)

