# Storing and loading constant data in the program space

This tutorial explains how to store and load constant in the program space, that is Flash memory.  
The ATMega 328P (found for example on Arduino Uno or Nano) has 32Kb of Flash memory (as opposed to 2Kb of Sram).   
Addressing scheme for Sram and Flash memory banks is on 16 bits, but :   
- Sram is organised in one byte chunck (8 bits).    
- On the other hand, Flash memory is organised in 2 bytes chunks ; this makes for 16365 possible addresses * 2 bytes = 32730 kB   
-> CPU instruction is 16 bits long on the ATMega 328P, so it does make complete sense. We can thus write up to 16365 assembly instructions, to simplify.

As SRAM is in short supply, using FLASH memory instead can be a good option. However, this space is read-only during program execution, so only **CONSTANTS** can be loaded and accessed in Flash.  
Below are exemples of how strings can be stored in FLASH and accessed using _lpm_ instruction (load from program memory) :  

```
;----------------------
; FLASH constant data
;----------------------
.section .rodata           ; this directive = load in program memory, not executable data
numerals:
    .byte 0x01, 0x02, 0x03
mon:
    .byte 'M','o','n','d','a','y',0
days:
    .byte 'S','u','n','d','a','y',0
    .asciz "Monday"
    .asciz "Tuesday"
    .ascii "Wednesday"
    .byte 0   ; ends string "Wednesday" with 0
    .asciz "Thursdayi"
    .asciz "Fridayi"
    .asciz "Saturday"
```
Note : Each FLASH addresses stores 2 bytes of data. This is why constant data must be _aligned_ (within 2 bytes boundaries).   
The following directives can lead to different behaviours:   
.ascii "Wed" ; because it is 3 bytes (odd), this directive will probably generates a compiler error.  
_.asciz_ directive adds a '0' at the end of the string and makes sure it is _aligned_. 

## Accessing data in the program space   

We access constant data in flash with the _lpm_ instruction, using the Z pointer (r30, r31) registers.

```
ldi ZH, hi8(numerals)   ; MSB of numerals FLASH address loaded in r31
ldi ZL, lo8(numerals)   ; LSB of numerals FLASH address loaded in r30
; Z now points at 0x01
lpm r16, Z+            ; 0x01 loaded in r16 ; Z address pointer is incremented (value in r30 + 1)
lpm r17, Z             ; 0x02 loaded in r17
```

**IMPORTANT**   
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
The explaination is that we must convert the 16 bits address into 8 bits addresses : 
```
Word address     BYTE addresses in Flash
------------     ------------------------------
0x0000        -> 0x0000 and 0x0001

0x0001        -> 0x0002 and 0x0003

0x0002        -> 0x0004 and 0x0005

0x0003        -> 0x0006 and 0x0007
```
=> BYTE address = WORD address * 2  
Suppose numerals: is located @ Flash word address 0x0123:  
Then the actual byte address in Flash is:   
0x0123 * 2 = 0x0246


So the code loads:   
```
ldi ZH, high(0x0246)
ldi ZL, low(0x0246)
```

(Arduino IDE asm compiler does the conversion automatically with the lo8 and hi8 macro.)

## Indirect addressing with displacement in Flash

There is no equivalent _ldd_ or _std_ with _lpm_. So to access a specific byte, we need some maths like in the following code :

```
table:
    .db 10, 20, 30, 40, 50

ldi ZH, hi8h(table)
ldi ZL, lo8(table)

adiw ZL, 3   ; shift address + 3
lpm r18, Z
```

Result : r18 now has 40

## Parsing a string in Flash   

We can parse a string this way :

```
loopchar:
  ldi ZH, hi8(mon) ; initialize Z pointer address
  ldi ZL, lo8(mon)
  lpm r16, Z+      ; load first byte @ lun: Flash address in r16 ; have Z points to next byte address
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
Please note that after this code Z will point to _mon:_ flash address + 5.  




