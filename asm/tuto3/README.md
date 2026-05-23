# Storing and loading in the program space

This tuto explains how to store and load constant in the program space, that is, Flash memory.  
The ATMega 328P (found for example on Arduino Uno or Nano) has 32Kb of Flash memory (as opposed to 2Kb of Sram). 
- Sram is organised in one byte chunck (8 bits).    
- On the other hand, Flash memory is organised in 2 bytes chunks : 16365 possible addresses * 2 bytes = 32730 kB
-> Each machine instruction is 16 bits long, so it does make complete sense. We can thus write 16365 assembly instructions, to simplify.

As SRAM is in short supply, using FLASH memory instead can be a good option. However, this space is read-only during program execution, so only CONSTANTS can be loaded and accessed in Flash.  
Below are exemples of how strings can be stored in FLASH and accessed using _lpm_ instruction (load from program memor) :  

```
;----------------------
; FLASH constant data
;----------------------
.section .text
numerals:
    .byte 0x01, 0x02, 0x03
lun:
    .byte 'l','u','d','i',0
jours:
    .byte 'D','i','m','a','n','c','h','e',0
    .asciz "Lundi"
    .asciz "Mardi"
    .ascii "Mercredi"
    .byte 0
    .asciz "Jeudi"
    .asciz "Vendredi"
    .asciz "Samedi"
```
Note : FLASH addresses are 16bits, and store 2 bytes. This is why constant data must be _aligned_ (within 2 bytes boundaries).   
.ascii "Mer" ; because it is 3 bytes (odd), this directive will probably generates a compiler error.  
_.asciz_ directive adds a '0' at the end of the string and makes sure it is _aligned_. 

We access constant data in flash with the _lpm_ instruction, using the Z pointer (r30, r31) registers.

```
ldi ZH, hi8(numerals)   ; MSB of numerals FLASH address loaded in r31
ldi ZL, lo8(numerals)   ; LSB of numerals FLASH address loaded in r30
; Z now points at 0x01
lpm r16, Z+            ; 0x01 loaded in r16 ; Z address pointer is incremented (value in r30 + 1)
lpm r17, Z             ; 0x02 loaded in r17
```

** IMPORTANT **
Remember :  

| Memory type            | Addressing               |
| ---------------------- | ------------------------ |
| SRAM (data memory)     | byte  (8 bits) addressed |
| Flash (program memory) | word (16 bits) addressed |

In old or different avr asm compilers, we may find the following code :   
```
ldi ZH, high(numerals*2)   ; 16 bits address converted to 8 bits
ldi ZL, low(numerals*2)    ;
```
The explaination is that we must convert a 16 bits address into 8 bits addresses : 
```
Word address     BYTE addresses in Flash
------------     ------------------------------
0x0000        -> 0x0000 and 0x0001

0x0001        -> 0x0002 and 0x0003

0x0002        -> 0x0004 and 0x0005

0x0003        -> 0x0006 and 0x0007
```
Suppose numerals: is located at Flash word address:  
```
numerals = 0x0123   ; word address
```
Then the actual byte address in Flash is:   
```
0x0123 * 2 = 0x0246
```

So the code loads:   
```
ldi ZH, high(0x0246)
ldi ZL, low(0x0246)
```

Arduino IDE asm compiler does the conversion automatically with the lo8 and hi8 macro.

We can parse a string this way :

```
loopchar:
  ldi ZH, hi8(lun) ; initiate Z pointer address
  ldi ZL, lo8(lun)
  lpm r16, Z+      ; load first byte at lun Flash address in r16
  tst r16          ; is r 16 = 0 ?
  breq end         ; then end of string
  rcall uart_send  ; call uart routine to send content of r16 to serial line (we suppose r16 will then be loaded in UDR0 register)
  rjmp loopchar    ; next byte

end:
  ldi r16, 0x13   ; send ascii CR (Carriage return) byte to uart routine
  rcall uart_send
  ldi r16, 0xoa   ; send ascii LF (Line Feed) byte to uart routine
  rcall uart_send
```
Please note that after this code Z will point to lun: flash address + 5.  




