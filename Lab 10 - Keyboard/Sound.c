// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on TM4C123
// Program written by: Connor Leu and Victor Olugbusi
// Date Created: 3/6/17 
// Last Modified: 1/11/22 
// Lab number: 6
// Hardware connections
/*
Key3=PE3, Key2=PE2, Key1=PE1, Key0=PE0, DAC=PB5-0
Key0=329.6, Key1=415.3, Key2=493.9, Key3=554.4 Hz
*/

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "dac.h"
#include "../inc/tm4c123gh6pm.h"


// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
	NVIC_ST_CTRL_R = 7;
	NVIC_ST_RELOAD_R = 0x00FFFFFF;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // from lecture, set priority
	DAC_Init();
}

// **************Sound_Start*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Start called again, or Sound_Off is called
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Start(uint32_t period){
	if (period == 0) {
		NVIC_ST_CTRL_R = 0;
		return;
	}
	NVIC_ST_RELOAD_R = period - 1;
	NVIC_ST_CTRL_R = 7; // enable + interrupts
}

// **************Sound_Voice*********************
// Change voice
// EE319K optional
// Input: voice specifies which waveform to play
//           Pointer to wave table
// Output: none
void Sound_Voice(const uint8_t *voice){
  
}
// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
	NVIC_ST_CTRL_R = 0;
}
// **************Sound_GetVoice*********************
// Read the current voice
// EE319K optional
// Input: 
// Output: voice specifies which waveform to play
//           Pointer to current wavetable
const uint8_t *Sound_GetVoice(void){
  return 0; // replace
}
#define PF4 (*((volatile uint32_t *)0x40025040))
#define PF3 (*((volatile uint32_t *)0x40025020))
#define PF2 (*((volatile uint32_t *)0x40025010))
#define PF1 (*((volatile uint32_t *)0x40025008))
#define PF0 (*((volatile uint32_t *)0x40025004))

// Interrupt service routine
// Executed every 12.5ns*(period)
uint8_t i = 0;

// From spreadsheet in lab6 folder
const uint8_t SinWave[64] = {				
  32,35,38,41,44,47,49,52,54,56,58,				
  59,61,62,62,63,63,63,62,62,61,59,				
  58,56,54,52,49,47,44,41,38,35,				
  32,29,26,23,20,17,15,12,10,8,				
  6,5,3,2,2,1,1,1,2,2,3,				
  5,6,8,10,12,15,17,20,23,26,29};				


void SysTick_Handler(void){
	DAC_Out(SinWave[i]);
	i = (i+1)%64;
}


