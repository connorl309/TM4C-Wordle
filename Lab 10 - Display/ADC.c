// ADC.c
// Runs on TM4C123
// Provide functions that initialize ADC0
// Last Modified: 1/12/2021  
// Student names: Connor Leu and Victor Olugbusi
// Labs 8 and 9 specify PD2
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function using PD2 
// Input: none
// Output: none
void ADC_Init(void){
	SYSCTL_RCGCADC_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x8;
	while ((SYSCTL_PRGPIO_R & 0x8) == 0) {}
	GPIO_PORTD_DIR_R &= ~0x4; // pd2 input
	GPIO_PORTD_AFSEL_R |= 0x4;
	GPIO_PORTD_DEN_R &= ~0x4; // disable digital pd2
	GPIO_PORTD_AMSEL_R |= 0x4;
	__nop();__nop();__nop();__nop();
	ADC0_PC_R &= ~(0x0F);
	ADC0_PC_R |= 0x01; 
	ADC0_SSPRI_R = 0x0123; 
	ADC0_ACTSS_R &= ~0x0008; 
	ADC0_EMUX_R &= ~0xF000;
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xfffffff0)+5;
	ADC0_SSCTL3_R = 0x0006;
	ADC0_IM_R &= ~0x0008;
	ADC0_ACTSS_R |= 0x0008;
}
//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
	ADC0_PSSI_R = 0x0008;
	while((ADC0_RIS_R&0x08) == 0) {}
	uint32_t result = ADC0_SSFIFO3_R & 0xFFF; // 12 bit result
	ADC0_ISC_R = 0x0008; // acknowledge
	return result;
}
