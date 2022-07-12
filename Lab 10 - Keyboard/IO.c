// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on TM4C123
// Program written by: put your names here
// Date Created: March 30, 2018
// Last Modified:  change this or look silly
// Lab number: 7


#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none
void IO_Init(void) {
 // --UUU-- Code to initialize PF4 and PF2
	SYSCTL_RCGC2_R |= 0x00000020; // 1) activate clock for Port F
	__nop(); __nop(); __nop(); __nop();
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
	GPIO_PORTF_CR_R = 0x1F; // allow changes to PF4-0
	// only PF0 needs to be unlocked, other bits can't be locked
	GPIO_PORTF_AMSEL_R = 0x00; // 3) disable analog on PF
	GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
	GPIO_PORTF_DIR_R = 0x0E; // 5) PF4,PF0 in, PF3-1 out
	GPIO_PORTF_AFSEL_R = 0x00; // 6) disable alt funct on PF7-0
	GPIO_PORTF_PUR_R = 0x11; // enable pull-up on PF0 and PF4
	GPIO_PORTF_DEN_R = 0x1F; // 7) enable digital I/O on PF4-0
}

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
 // --UUU-- PF2 is heartbeat
	GPIO_PORTF_DATA_R ^= 0x2;
}


//------------IO_Touch------------
// wait for release and press of the switch
// Delay to debounce the switch
// Input: none
// Output: none
void IO_Touch(void) {
 // --UUU-- wait for release; delay for 20ms; and then wait for press
	while (GPIO_PORTF_DATA_R >> 4 == 0) {} // pressed = 0
	int counter = 800000; // 80Mhz clock, ~1.6 mil executions for 20ms delay
	while (counter-- > 0) {}
	while (GPIO_PORTF_DATA_R >> 4 == 1) {} // unpressed = 1
}  

