# Understanding ATMega328P hardware #

(Please read the ATMega328P datasheet up to part 7)

<img width="497" height="573" alt="ATMega328p" src="https://github.com/user-attachments/assets/67789112-f1aa-49a4-88a0-6d92d0a501c1" />

Three main components are highlighted : the FLASH memory, the SRAM memory, and the ALU (aka CPU)  
Simple architecture view :

                CPU
          +----------------+
          | Registers      | -> FLASH
          | R0..R31        |
          +----------------+
                  |
          -------------------
          |        |        |
          v        v        v

        SRAM    I/O Regs   Timers/UART/etc
                  |
               GPIO Pins




The ATMega328P has the following caracteristics : 

Program address space :  
- 32 kbytes of ==FLASH== store the code and constants (like strings etc.)
  -   Address from 0x0000 0x3FFF(0 to 16283)
  -   Organized in 16K * 16 bits (each address location stores 16 bits). Each code instruction is 16bits wide - cf below
  -   The boot flash section, if any, is stored at the end of the Flash (boot flash section). Its size is specified in the fuse bits -  which are stored in a dedicated memory module.
  -   The program counter will point to the next executable instruction address in the FLASH.

Data address space:
- Addess from 0x000 to 0x8FF (0 to 2303)
- Organized in 2K * 8 bits (each address location stores 8 bits) and includes :
  - 32 General Purpose Register (r0 -> r31), directly wired to the ALU (Arithmetic and logical unit, aka CPU).
  - 64 I/O registers (GPIO etc.)
  - 160 Extended I/O registers
  - 2 kbytes of ==SRAM== to store variables, pointers etc.

Depsite being physically different, they all share the same addressing scheme. However note the following :  
0x0000 to 0x0020 (32) are the General Purpose Registers addresses. 
Immediate instuctions (ldi, andi, subi, ori etc.) are valid only on r16 to r31  
The 64 other registers, or special file register, are often addressed relatively from the last GPR address (0x20), as seen in the diagram above. 

**IMPORTANT** :
- I/O registers within the address range 0x00 - 0x1F are directly bit-accessible using the SBI and CBI instructions. In these registers, the value of single bits can be checked by using the SBIS and SBIC instructions.

<img width="703" height="609" alt="bit-io-registers" src="https://github.com/user-attachments/assets/ebd297df-b629-42b0-a350-06e3955736e6" />


- When using the I/O specific commands IN and OUT, the I/O addresses 0x00 - 0x3F must be used.
- When addressing I/O registers as data space using LD and ST instructions, 0x20 must be added to these addresses.

  The diagram below shows the data memory structure more in details. This is important because some assembly instructions will only work within a specific space.

  <img width="1421" height="545" alt="data_memory" src="https://github.com/user-attachments/assets/5652a39a-e08e-4fef-8c31-c438cee75b05" />


  To sum up :  
  Data Memory :  
- Registers             : 0x0000 - 0x001F - 32 GPR
- I/O registers         : 0x0020 - 0x005F
- Extended I/O register : 0x0060 - 0x00FF
- SRAM                  : 0x0100 - RAMEND (0x8FF for ATMega328P)



  Hint : to see how an Arduino or ATMega programmed in C translates in assembly, do
```
  avr-objdump -d my_program.elf
```


