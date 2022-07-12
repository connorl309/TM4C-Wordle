// UART.c
// Runs on LM4F120/TM4C123
// Provides Prototypes for functions implemented in UART.c
// Last Modified: 3/6/2015 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "tm4c123gh6pm.h"

// UART initialization function 
// Input: none
// Output: none
void UART_Init(void);

//------------UART_InChar------------
// Wait for new input, interrupt driven
// then return ASCII code
// Input: none
// Output: char read from UART
unsigned char UART_InChar(void);

//------------UART1_InMessage------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until ETX is typed
//    or until max length of the string is reached.
// Input: pointer to empty buffer of 8 characters
// Output: Null terminated string
void UART_InMessage(char *bufPt);

//------------UART_OutChar------------
// Wait for new input,
// then return ASCII code
// Input: none
// Output: char read from UART
// *** Need not be busy-wait if you know when to call
void UART_OutChar(char data);

void UART_OutString(char *agony);
