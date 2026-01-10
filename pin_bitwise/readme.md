<h2 align="center">Bitwise PIN operation</h2>

<h3>GPIO states reminder</h3>

#### GPIO States

| DDxn | PORTxn | State Name / Electrical Behavior      |
|------|--------|--------------------------------------|
| 0    | 0      | High-Z Input / Floating input           |
| 0    | 1      | Input with pull-up (default high state if not grounded)      |
| 1    | 0      | Output low (sink current)               |
| 1    | 1      | Output high (source current)              |


To manage a pin with an Arduino, we would most often use the well known functions :<br>
- pinMode(pin, OUTPUT|INPUT|INPUT_PULLUP);
- digitalWrite(pin, 0|1); // Output LOW or HIGH if pin set to output
- digitalRead(pin);       // Read the state of a pin in INPUT mode (return 0 if LOW or 1 if HIGH )

INPUT_PULLUP will connect the PIN (in INPUT mode) to an internal "pull up resistor" ; that is a resistor (between 20K Ohms and 50K Ohms - around 40K Ohms on one of my Arduino Nano) connected to VCC (5V here).<br>
The PULL UP resistor will ensure the PIN is in a default known state (HIGH), and not "floating". When connected to ground, the pin will source a tiny current : VCC / R = around 100-125µA

![Pin-pull-up](https://github.com/user-attachments/assets/6b77059a-ac19-4ff4-aa4d-36667e929e88)

These Arduino functions actually induce some delays. For example a digitalWrite() can take up to 10µs between two immediate calls of this function (Arduino Nano @ 16MHz).

Another and quicker way is to access the ATMega328P GPIO registers directly. There are three registers to consider : 
- DDRn
- PORTn
- PINn <br>
Each register is 8 bits long (numbered 0 to 7), so it can handle 8 pins. On an Arduino Nano, use the following ports registers / pins map :

PD = D0, D1, D2, D3, D4, D5, D6, D7 <br>
PB = D8, D9, D10, D11, D12 et D13 <br>
PC = D14, D15, D16, D17, D18, D19 <br>

Note : the "D" in D0, D1 etc. here stands for "Digital", as opposed to "Analog", and can be used ONLY in Arduino functions.

On an Arduino Nano, PD controls the first 8 pins, that is PD0 to PD7 (IMPORTANT !! PD0 is also used for serial Rx, and PD1 for serial Tx - DO NOT USE these pins)<br>

- DDRn will set the INPUT/OUTPUT mode of a pin : 0 is input, 1 is output. Below is the register for DDRD (controls pins PD0 to PD7) :

![arduino-ddrd](https://github.com/user-attachments/assets/b1d63d68-c92a-45e7-b378-eb1714867b40)

We can see that by default all pins are in input mode (0).<br>

- PORTn will set a pin in OUTPUT mode to LOW (0) or HIGH (1). Below is the register for PORTD (controls pins PD0 to PD7)  :

![arduino-portd](https://github.com/user-attachments/assets/7e51279b-d930-4802-a04b-9de6dd5ecf04)

We can see that by default all pins are in LOW mode (0).<br>

- PINn will read the state of a pin in INPUT mode : HIGH (1) or LOW (0). Below the register for PIND  (controls pins PD0 to PD7)   :

![arduino-pind](https://github.com/user-attachments/assets/c0a87e39-3040-4581-940d-250b5cc9f855)

- Note that this register is read-only. But for curiosity, see end of this note.

<h3 align="center">General operations on registers</h3>

You can read a [bitwise tutorial](https://docs.arduino.cc/learn/programming/bit-math/) if you are not familiar with &, |, ~, ^ bitwise operator (AND, OR, NOT, XOR) : 

!! Important : when playing with ATMega328P registers, use avr/io.h definitions : !! <br>
ie :<br>
#define PORTD5 5  // This define the bit position in PORTD register<br>
#define PD5    5  // This does exactly the same ;-)<br>

!! Do NOT use Arduino pin convention like D5 etc. in your code. They have no meaning at this lower level !!

1. To set Arduino Nano pin D5 in input mode :
   - DDRD bit 5 must be set to 0 (input mode).

2. To set (or not) the PULLUP resistor on Arduino Nano pin D5 : 
   - DDRD bit 5 must be set at 0 (input mode).
   - PORTD bit 5 must be set to 1 (activate PULLUP) ; by default  set to 0, no PULLUP <br>
(Note : there is a register that disable all PULLUP resistors - bit 4 in the MCUCR register -  it will be ignored here)

3. To read the state of Arduino Nano pin D5 in input mode, use PIND register :
   - If bit 5 reads 1, the input is HIGH (at VCC 5V, input must be above 3V)
   - If bit 5 reads 0, the input is LOW (at VCC 5V, input must be below 1,5V)      

4. To set Arduino Nano pin D5 in output mode :
   - DDRD bit 5 must be set to 1 (onput mode).
   - PORTD bit 5 can then be set to 1 HIGH (5V, source up to 40 mA) ; or set to 0 LOW (ground, can sink up to 40mA).

  
  <h3 align="center">Examples</h3>
  
 1.  Put Arduino Nano pin D5 in output mode ; several methods supposing default DDRD register = 0b00000000 :<br>
    - DDRD |= 0b00100000; // Set bit 5 (PD5) as OUTPUT (this syntax is equivalent to : DDRD = 0b00000000 | 0b00100000)<br>
    - DDRD |= (1<<5);<br>
    - DDRD |= (1<<PD5);<br>
    (0b00100000 is called a 'mask' ; DDRD can then be "ORED", "ANDED" etc. with this mask).<br>
    The first option is pretty straight forward : it does an OR on DDRD register 0b00000000 with 'mask' 0b00100000 =>  DDRD becomes 0b00100000<br>
    The second and third option involves two operations :
    - First : do a bitwise shift to the left<br>
      - Each shift left moves 1 to the next bit position:<br>
        1 << 0 → 00000001<br>
        1 << 1 → 00000010<br>
        1 << 2 → 00000100<br>
        1 << 3 → 00001000<br>
        1 << 4 → 00010000<br>
        1 << 5 → 00100000 (our mask result)<br>
        1 << 6 → 01000000<br>
        1 << 7 → 10000000<br>
     - Second : we have the same operation as the first option :  it does an OR on DDRD with the mask = 0b00000000 | 0b00100000 = 0b00100000<br>
     Note that the OR | operator leaves the other bits unchanged whether they are 1 or 0 :<br>
     - Suppose we had initially DDRD = 0b10000000 :  0b10000000 | 0b00100000 = 0b10100000<br>
     
3. Put Arduino Nano pin D5 HIGH (5V) => set bit 5 = 1 :<br>
   - PORTD |= 0b00100000;
   - PORTD |= (1<<5);
   - PORTD |= (1<<PD5);<br>
   The logic is the same as setting a DDRD bit to 1. Other bits left unchanged.

4. Set Arduino pin D5 LOW (groud) :<br>
This involves three operations for the last two options ; supposing PORTD register = 0b00100000 :
   - PORTD &= ~0b00100000; // Set Arduino Nano pin D5 LOW
   - PORTD &= ~(1<<5);
   - PORTD &= ~(1<<PD5);<br>
     - First : SHIFT 1 to 5th position => 0b00100000 - this will be our initial mask.
     - Second : NOT the mask (flips all bits of the mask) = 0b11011111
     - Third : AND with PORTD register :  0b00100000 &  0b11011111 => PORTD = 0b00000000
    Again, other bits of PORTD register set to 1 left unchanged since 1 AND 1 = 1
    
5. Toggle Arduino pin D5 : <br>
Three options as usual :
   - PORTD ^= 0b00100000;
   - PORTD ^= (1<<5);
   - PORTD ^= (1<<PD5);
   Here WE XOR the bits : 1 XOR 1 = 0 and 0 XOR 0 = 0 but 0 XOR 1 = 1<br>
   Suppose PORTD = 0b00100000 XOR 0b00100000 => PORTD = 0b00000000 <br>
   Suppose PORTD = 0b00000000 XOR 0b00100000 => PORTD = 0b00100000 <br>

6. Read Arduino pin D5 status : <br>
Suppose DDRD = 0b00000000 ; all pin INPUT mode
   - We set a variable : uint8_t status;
   - status = (PIND & 0b00100000) >> 5; // 0b00100000 is the mask for pin at bit 5 position
   - status = (PIND & 0b00100000) >> PD5;<br>
Suppose PIND register 0b00100110 : 0b00100110 & 0b00100000 = 0b00100000 :<br>
     Shift five positions to right : 0b00100000 >> 5  = 0b00000001 = 1 <br>
     Status = 1;<br>
Suppose PIND register 0b00000110 : 0b00000110 & 0b00100000 = 0b00000000<br>
     Shift five positions to right : 0b00000000 >> 5  = 0b00000000 = 0 <br>
     Status = 0; <br>
Note : you dont really need actually to shift bits here ; you can just test like (status)?(_expression_true_);_expression_false_; 



  <h3 align="center">Even more simple</h3>

  Arduino proposes several macro to perform bit operations (rather fast actually) :
  1. bitSet(byteName, n)  :<br>
     - bitSet(PORTD, PD5); // Set PORTD 5th bit to 1
  2. bitClear(byteName, n) <br>
     - bitClear(PORTD, PD5); // Set PORTD 5th bit to 0
  3. bitRead(byteName, n) : <br>
     - bitRead(PORTD, PD5); // Read PORTD 5th bit status
    
  <h3 align="center">Somewhat twisted</h3>
  
  Arduino also proposes the bit() macro, that can be used this way :
  - PORTD ^= bit(PD5);                     // toggle state of digital pin 5
  - PORTD ^= bit(PD5) | bit(PD3);          // toggle state of digital pins 5 & 3
  - PORTD &= ~bit(PD5);                    // set digital pin 5 as LOW
  - PORTD |= bit(PD5);                     // set digital pin 5 as HIGH
  - DDRD &= ~bit(PD5);                     // set digital pin 5 as INPUT
  - DDRD &= ~bit(PD5); PORTD |= bit(PD5);  // set digital pin 5 as INPUT_PULLUP
  - DDRD |= bit(PD5);                      // set digital pin 5 as OUTPUT

    <h3 align="center"> Final tweak</h3>

    The ATMega328P datasheet states :
    13.2.2 Toggling the Pin <br>
    Writing a logic one to PINxn register toggles the value of PORTxn, independent on the value of DDRxn.<br>
    So :<br>
    - PIND = bit(PD5);    // HIGH
    - PIND = bit(PD5);    // LOW
    - PIND = bit(PD5);    // HIGH
   
<h3 align="center"> Caution </h3>

Safe GPIO Transitions (Simplified -  see 13.2.3 Switching Between Input and Output of ATMega328P datasheet)

- When switching **Tri-state (High-Z INPUT) to OUTPUT HIGH**, set an **intermediate OUTPUT LOW**.
- When switching **INPUT with PULLUP to OUTPUT LOW**, set an **intermediate INPUT HIGH-Z (INPUT, no PULLUP)**.

---

  #### Below a schematic for the code used here, from SimulIDE :<br>

  
![bitwise-simulide](https://github.com/user-attachments/assets/75e7f9c1-3691-499f-9087-165a30c1370f)

    







