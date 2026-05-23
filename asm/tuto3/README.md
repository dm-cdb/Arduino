# Storing and loading in the data space

This tuto explains how to store and load constant in the program space (Flash memory).  
SRAM is in short supply, so using FLASH memory instead can be a good option.   
However, this space is read-only during program execution, so only CONSTANT can be loaded and accessed. We'll find therefore mostly strings for error messages etc.  
Below are exemple of how strings can be stored in FLASH :  

```
;----------------------
; FLASH constant data
;----------------------
.section .data
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
Note : FLASH addresses are 16bits, and store 2 bytes. This is why it must be aligned (with 2 bytes).   
.ascii "Mer" ; because it is 3 bytes (odd), this directive will probably generates a compiler error.  
.asciz directive adds a '0' at the end of the string.  

We access constant data in flash with the _lpm_ instruction, again using the Z pointer (r30, r31).

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

In old or different avr asm compiler, we may find the following code :   
```
ldi ZH, high(numerals*2)   ; 16 bits address converted to 8 bits
ldi ZL, low(numerals*2)    ;
```
The explaination is that we must convert a 16 bits address into two 8 bits addresses.   

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




