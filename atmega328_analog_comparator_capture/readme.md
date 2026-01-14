<h3 aligh="center"> Arduino Nano Analog comparator with TIMER1</h3>

Normaly the output analog comparator will be tied to the ACO bit of the ACSR register.<br>
However there is also the possibility to redirect the comparator output to TIMER1 block, in place of the ICPn pin of the ATMega328P normally used. See diagram below :


![Analog-comp-to-TIMER1](https://github.com/user-attachments/assets/2751dbd9-ea7e-468f-89d8-e0240737d6aa)

The the output of the comparator will be treated exactly as the ICPn pin. Everything from there will be handled by TIMER1 registers.<br>
Here we have :<br>
- Vdd = 5V
- AIN0 = 2,5V (10k or why not 100k resistors voltage divider).
- AIN1 = AC signal between 0v and 5v.

This project will check that the incoming signal at AIN1 is around 500Hz.

TIMER1 is configured with a prescaler of 1:8. This means that @16MHz, TIMER1 will see a frequency of 2MHz, that is 0,5µs per tick.<br>
As TIMER1 has a 16bits counter register, it will overflow each 65535 * 0,5µs = 32767µs = 0,033ms<br>
As the period of a 500Hz signal is 1/500 = 2ms, it means we can read around 16 periods before TIMER1 overflows.<br>
Each rising edge of the signal will generate an interrupt, and the content of TIMER1 counter register will be copied to the ICR1 register in the interrupt routine.<br>
We just measure the interval between two interrupts and we have the period in TIMER1 ticks.<br>
As each TIMER1 tick = 0,5µs, we multiply the number of ticks by 0,5 (or divide by two) to have the period in µs.<br>
From there we can calculate the frequency on AIN1.<br>

We can also have a counter that will display the result each period * counter. Just adjust to your need.<br>

And that's it.

