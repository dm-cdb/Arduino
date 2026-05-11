# Transfering data 
This tuto explains how to access and move data within the ATMega328P environment.  
Depending on the hardware involved (Flash, SRAM, Special File Register, General Purpose Register) different instructions can (or must) be used.   
The diagram below illustrates the complexity of accessing the data memory banks :  
<br>

<img width="1421" height="545" alt="data_memory" src="https://github.com/user-attachments/assets/c968cf79-4b1d-41a7-a137-ed320f6f08b9" />


## Accessing the General Purpose Registers (r0-r31)
1. _ldi_   
- To load a constant in a GPR, you must use the following instruction : _ldi_   
- Only registers r16 -> r31 support this instruction.
- For example, to load constant 15 in GPR, do :
```
ldi r16, 15 (or 0x0f, or 0b00001111)
```
2. Using r0...r15   
- Loading constant K in, say, r0, will be a two steps operation, involving the _mov_ instruction   
```
ldi r16, 127
mov r0, r16  ; IMPORTANT : mov can only be used between General Purpose Registers r0...r31
```
- Likewise, immediate instructions _subi_, _ori_, _andi_, _cpi_... work only with GPR r16 -> r31   

## Accessing I/O registers
1. _in/out_   
- The first 64 SFR can be accessed with _in_/_out_ instruction   
- Suppose we want to set DDRD register with 11110000 (bits 0-3 as input, bits 4-7 as output):

```
ldi r16, 0xf0
out DDRD, r16
```
- Or we need to load r16 with the content of DDRD   

```
in r16, DDRD
```

- _in_ means transfer SFR content into ALU registers.  
- _out_ means transfer content from ALU registers to SFR.  
- Remember : _in/out_ can only access the first 64 SFR (from relative address 0x000 to 0x03f (the SREG address)   

2. _lds/sts_   
- _lds_ and _sts_ cover the whole SRAM address range.
- We need to use the absolute addressing here : ie PINB relative address 0x003 will be absolute address 0x003 + 0x020 = 0x023   
- For example, to load DDRD with content of r16   

```
ldi r16, 0xf0
sts DDRD, r16
```
To load r16 with the content of DDRD
```
lds r16, DDRD
```
Note : _sts/lds_ instruction costs more cpu cycles than _in/out_

3. _ld/st_ store and load indirect   
Here we will use pointer register X, Y or Z to store an address in SRAM :
- X = r26-r27
- Y = r28-r29
- Z = r30-r31
<br>
Suppose we reserve 4 bytes in SRAM address space (from 0x100) :   

```
.section .bss  ; SRAM data space
var:
  .space 4

 .text        ; Flash program space
ldi r30, lo8(var) ; load Z pointer r30-r31 with var SRAM address - lower 8 bits in r30, higher bits in r31
ldi r31, hi8(var)

ldi r16, 0x0a
st Z+, r16 ; 0x0a stored @var and Z pointer address incremented (ie if initially Z = 0x100 -> Z = 0x101)
inc r16    ; 0x0b stored @var + 1
st Z+, r16
inc r16    ; 0x0c stored @var + 2
st Z+, r16
inc r16    ; 0x0d stored @var + 3
st Z, r16

ld r20, Z  ; 0x0d stored in r20
ld r21, -Z ; Z decremented, then content addressed by Z stored in r21 (0x0c)
ld r22, -Z ; Same operation -> r22 = 0x0b
ld r23 -Z  ; Same operation -> r23 = 0x0a
```

- Note that Z is changing after each Z+, or -Z   
- Same result can be achieved through indexing with std or ldd :   

```
.section .bss  ; SRAM data space
var:
  .space 4

 .text        ; Flash program space
ldi r30, lo8(var) ; load Z pointer r30-r31 with var SRAM address - lower 8 bits in r30, higher bits in r31
ldi r31, hi8(var)

ldi r16, 0x0a
st Z, r16  ; 0x0a stored @var
inc r16    ; 0x0b stored @var + 1
std Z + 1, r16
inc r16    ; 0x0c stored @var + 2
std Z + 2, r16
inc r16    ; 0x0d stored @var + 3
std Z + 3, r16

ldd r20, Z + 2  ; r20 = 0x0c
ldd r21, Z + 1  ; r21 = 0x0b
ld r22, Z       ; r22 = 0x0a  ; no offset - we simply use ld
ldd r23, Z + 3  ; r23 = 0x0d
```
- The allowed displacement range is 0 < q < 63.  
- Z does not change after an _ldd_ or _std_ instruction (ie r30-r31 remains unchanged).
