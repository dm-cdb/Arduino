# Transfering data 
This tuto explains how to access and move data within the ATMega328P environment. Depending on the hardware involved (Flash, SRAM, Special File Register, General Purpose Register) different instruction can (or must) be used.   
The diagram below illustrate the complexity of accessing the data memory banks :  
<br>

<img width="1421" height="545" alt="data_memory" src="https://github.com/user-attachments/assets/c968cf79-4b1d-41a7-a137-ed320f6f08b9" />


## Accessing the General Purpose Registers (r0-r31)
1. Immediate or loading constant K in GPR
- To load a constant in a GPR, you must use the following instruction : _ldi
However, only registers r16 -> r31 support this instruction. For example, to load constant 15 in GPR, do :
```
ldi r16, 15 (or 0x0f, or 0b00001111)
```
2. Using r0...r15
- To load a constant K in, say, r0, this is a two step operation, invlving the _mov instruction
```
ldi r16, 127
mov r0, r16  ; IMPORTANT : mov can only be used between General Purpose Registers r0...r31
```
- Likewise, immediate instructions _subi, _ori, _andi, _cpi... work only with GPR r16 -> r31

## Accessing I/O registers
1. in/out
Suppose we want to set DDRD register with 11110000 (bits 0-3 as input, bits 4-7 as output). This would be a two step operations :
```
ldi r16 0xf0
out DDRD, r16
```
Or we need to load r16 with the content of DDRD
```
in r16, DDRD
```
So :  
_in means input SFR content into ALU registers.  
_out means output content from ALU registers to SFR.  
_in/_out can only access the first 64 SFR (from relative address 0x000 to 0x03f (the SREG address)   

2. lds/sts
_lds and _sts cover the whole SRAM address range. We need to use the absolute addressing here :
PINB relative address 0x003 will be absolute address 0x003 + 0x020 = 0x023
To load DDRD with content of r16
```
sts DDRD, r16
```
To load r16 with the content of DDRD
```
lds r16, DDRD
```
Note : sts/lds instruction costs more cpu cycles than in/out


4. 
