# Understanding ATMega328P hardware #

(Please read the ATMega328P datasheet up to part 7)  

<img width="1536" height="1024" alt="AtMega328p-architecture-v2" src="https://github.com/user-attachments/assets/ed73cf6f-48cf-44c1-8b82-031eea0e4bec" />  


Three main components are highlighted : the FLASH memory, the SRAM memory, and the ALU (aka CPU)  

The ATMega328P has the following caracteristics : 

Program address space :  
- 32 kbytes of <mark>FLASH</mark> that store the code and constants (like strings etc.) This memory is read-only.
  -   Address from 0x0000 0x3FFF (0 to 16283)
  -   Organized in 16K * 16 bits (each address location stores 16 bits), that is, each code instruction is 16bits wide - see below
  -   The boot flash section, if any, is stored at the end of the Flash. Its size is specified by the fuse bits -  which are stored in a dedicated memory module.
     - The bit fuses can only be accessed during programming the microcontroller.
  -   The program counter will point to the next executable instruction address in the FLASH.

Data address space:
- Addess from 0x000 to 0x8FF (0 to 2303) ; this memory is read & write.
- Organized in 2K * 8 bits (each address location stores 8 bits) and includes :
  - 32 General Purpose Register (r0 -> r31), directly wired to the ALU (Arithmetic and logical unit, aka CPU).
  - 64 I/O registers (GPIO etc.), also names SFR (Special File Register)
  - 160 Extended I/O registers (located in SRAM).
  - 2 kbytes of <mark>SRAM</mark> to store variables, the stack etc.

The data space, depsite being physically dispatched on different physical modules, has a single addressing scheme (0x000 to 0x8ff). However note the following :  
- 0x000 to 0x01F (32) are General Purpose Registers addresses.  
  - "Immediate" instructions (ldi, andi, subi, ori etc.) can only be used on GPR r16 to r31.
- 0x020 to 0x05F addresses store the first 64 I/O registers. They are often addressed relatively from the last GPR address (0x01F).
- 0x060 to 0x0FF addresses store extended SFR (in SRAM)
- 0x0100 to 0x08FF addresses are available SRAM for the stack, variables etc.  
Note : Data in SRAM is not directly accessible by the ALU ; one must first load/store their content into/from a GPR like r3 or r16.  

<p>
<img width="703" height="609" alt="bit-io-registers" src="https://github.com/user-attachments/assets/ebd297df-b629-42b0-a350-06e3955736e6" />  
</p>

**IMPORTANT** :  
- I/O registers within the address range 0x00 - 0x1F are directly bit-accessible using the SBI and CBI instructions. In these registers, the value of single bits can be checked by using the SBIS and SBIC instructions.
- When using the I/O specific commands IN and OUT, the I/O relative addresses 0x000 - 0x03F must be used.
- When addressing I/O registers as data space using LD and ST instructions, absolute addesses must be used.

  The diagram below shows the data memory structure more in details. This is important as some assembly instructions will only work within a specific space.

  <img width="1421" height="545" alt="data_memory" src="https://github.com/user-attachments/assets/5652a39a-e08e-4fef-8c31-c438cee75b05" />


  To sum up :  
  Data Memory :  
- GP registers          : 0x0000 - 0x01F = 32 GPR
- I/O registers         : 0x0020 - 0x03F - 0x05F = 32 + 32 SFR (or 0x000 - 0x001F - 0x003F in relative addressing)
- Extended I/O register : 0x0060 - 0x0FF = 160 SFR
- SRAM                  : 0x0100 - RAMEND (0x8FF for ATMega328P)

  Hint : to see how an Arduino or ATMega programmed in C translates in assembly, do
```
  avr-objdump -d my_program.elf
```

## A word or two about machine code ##
Programming in assembly is slightly different from programming in machine code !  
Machine code is a succession of bits (16 for the ATMega328p) representing the instruction and the operands.  
For exemple, the ANDI instruction does a logical AND on register r16<->r31 and a constant, for example 0x0f  
The 16 bits machine code will be :
- 4 bits for the instruction code andi : 0111
- 4 bits for the constant (K)
- 4 bits for the register address (d) (minus 16, for the immediate instructions only work from r16 -> r31 : 1111 = 15)
- 4 bits for the constant (K)

=> So its opcode will be 0111 KKKK dddd KKKK

ie :  
<code>andi r16, 0x0f
16 - 16 = 0 =>
0111 0000 0000 1111
=> machine code = 0x700f
=> stored in FLASH as 0f 70 (little endian byte coding, that is LSB then MSB)
</code>

Other examples : 

<mark>The SBI instruction</mark> set a bit in an I/O register.  
Its opcode is 1001 1010 AAAA Abbb  
- 8 bits for the instruction code
- 5 bits for the I/O register address (we understand now why we can only reach the first 32 SFR's : 11111 = 1F = 31)  
- 3 bits for the bit address to be set

ie :   
<code>sbi DDRB, 3  
AAAA is 0100 (0x04 address using the relative addressing scheme)  
bbb is 011
1001 1010 0010 0011
=> machine code = 0x9A23
=> stored in FLASH as 23 9a
</code>  


<mark>The BCLR instruction</mark> clear a bit in the SREG register (the status register located at 0x3F - the last 64 I/O register)  
Its opcode is 1001 0100 1sss 1000  
- 13 bits for the instruction code
- 3 bits for the bit position to clear (s)

ie :   
<code>bclr 5
1001 0100 1101 1000
=> machine code = 0x94d8
=> stored in FLASH as d8 94
</code>

<mark> the IN instruction </mark>copy an SFR content into a GPR.  
Its opcode is 1011 0AAd dddd AAAA  
- 5 bits for the instruction code
- 6 bits for the SFR address (note how the address is sprayed on 2 bytes ; also 111111 = 63 = 0x3F = max usable address)
- 5 bits for the register address (11111 = 31 = 0x1F)

ie :   
<code>in r13, SREG  
r13 = 01101
SREG = 111111 = 0x3F
1011 0110 1101 1111
=> machine code = 0xb6df
=> stored in FLASH as df b6
</code>

<mark>The ADD instruction</mark> add two GPR, storing the result in the destination register.  
Its opcode is 0000 11rd dddd rrrr  
- 6 bits for the instruction code
- 5 bits for the source register (r) (note how the r address is sprayed on two nibbles)
- 5 bits for the destination register (d)

ie :  
<code>add r3, r16 ; add content of r16 (source r) with r3 (destination d), store result in r3  
d = r3  = 00011  => first bit goes to 8th position from left of the opcode
r = r16 = 10000  => first bit goes to 7th position from ledt of the opcode
0000 1110 0011 0000
=> machine code = 0x0e30
=> stored in FLASH as 30 oe
</code>
