// DAC.c
// This software configures DAC output
// Lab 6 requires 6 bits for the DAC
// Runs on LM4F120 or TM4C123
// Program written by: Connor Leu and Victor Olugbusi
// Date Created: 3/6/17 
// Last Modified: 1/11/22 
// Lab number: 6
// Hardware connections
/*
Key3=PE3, Key2=PE2, Key1=PE1, Key0=PE0, DAC=PB5-0
Key0=329.6, Key1=415.3, Key2=493.9, Key3=554.4 Hz
*/

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data



// **************DAC_Init*********************
// Initialize 6-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x2; // port E and B
	__nop(); __nop();
	__nop(); __nop();
	GPIO_PORTB_DIR_R = 0x3F; // PB5-0
	GPIO_PORTB_DEN_R = 0x3F;
	GPIO_PORTB_DR8R_R |= 0x3F;
}

// **************DAC_Out*********************
// output to DAC
// Input: 6-bit data, 0 to 63 
// Input=n is converted to n*3.3V/63
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R = (uint8_t)data;
}
