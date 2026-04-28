This directory contains small programs in assembly for the Arduino based on the ATMega328P.
There are two files using Arduino IDE : a '.ino' file and a '.S' file.
The .ino file contains the usual C++ directives and setup() / loop() functions. Functions used in the .S files are first declared in the extern "C" bloc.
This compiler/linker directive is necessary so that the function names declared inside are not mangled by the c++ compiler. The extern directive really means : "the functions enclosed are declared in another file and are not in C++, don't mess with their names inside."
One can also declare a global variable if needed.
Ex :
asm.ino
//-------------------------------------------
// Assembly via Arduino
//-------------------------------------------
extern "C"
{
  void myAsm_func();
}
//-------------------------------------------
void setup()
{
  myAsm_func();
}
//-------------------------------------------
void loop(){}

Note : 
- The function parameters (if any) are stored in registers r25 … r8, the first byte being stored in r24.
- if the paramater is :
- uint8_t, it will be stored in r24 - next uint_8t will stored in even registers (ie r22, r20 etc.)
- uint16_t will be stored in r24-r25
- unint32_t will be stored in r22-r23-r24-r25
- The return value is passed in r25-r18, depending on the size of the return value (maximum return value size: 8 bytes).
- if the return value is 1 byte, it is placed in r24.

asm.S
//-------------------------------------------
// Assembly code
//-------------------------------------------
#define __SFR_OFFSET 0x00
#include "avr/io.h"
;------------------------
.global myAsm_func
;===============================================================
; begin code
;===============================================================
myAsm_func:

