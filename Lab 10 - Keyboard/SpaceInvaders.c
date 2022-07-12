// SpaceInvaders.c
// Runs on TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 1/12/2022 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php

// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)
// LED on PD1
// LED on PD0


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "Images.h"
#include "Timer1.h"
#include "Timer0.h"
#include "Uart.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// All custom stuff here

void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}

// Keyboard row
enum KeyboardRow
{
	TOP = 0,
	MIDDLE = 1,
	BOTTOM = 2
};

uint32_t ConvertADC(uint32_t adc_value); // converts ADC value to a valid keyboard row character index
uint32_t StringLen(const char *string);
void DrawLetterOutline(enum KeyboardRow row, uint8_t index);

uint8_t KeyIndex = 0; // the key index for the specified keyboard row
uint8_t OldIndex = 0; // the previous selected key (for updating screen)

enum KeyboardRow oldRow;
enum KeyboardRow keyRow;
// Keyboard strings
char TopRow[] = "QWERTYUIOP";
char MiddleRow[] = "ASDFGHJKL";
char BottomRow[] = "ZXCVBNM";

void Timer1A_Handler(void){ // can be used to perform tasks in background
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
   // execute user task
}

// SysTick init function
void SysTick_Init(uint32_t period) {
	if (period == 0) {
		NVIC_ST_CTRL_R = 0;
		return;
	}
	NVIC_ST_RELOAD_R = period - 1;
	NVIC_ST_CTRL_R = 7; // enable + interrupts
}

// String length function (thanks exam 2 lol)
uint32_t StringLen(const char *string) {
	uint32_t size = 0, index = 0;
	while (string[index] != 0) { size++; index++; }
	return size;
}

int main(void) {
  DisableInterrupts();
	// I've disabled UART0 init inside this function
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
	
  Output_Init(); // Takes care of LCD
	ST7735_InvertDisplay(1); // Fix my LCD colors to be normal
	
	ST7735_SetTextColor(ST7735_WHITE); // Text color to white
	ST7735_OutString("Starting Lab 10:\nConnor Leu and\nVictor Olugbusi");
	SysTick_Init(80000000/10); // 10 Hz
	ADC_Init(); // ADC Sampling, input into PD2
	UART_Init(); // UART init - rework to only send 1 character; Send on PC4
	Random_Init(NVIC_ST_CURRENT_R); // Seed Random() (we dont *really* use it but its fun to have :) )
	
	//https://www.ti.com/lit/ds/spms376e/spms376e.pdf?ts=1650294620415&ref_url=https%253A%252F%252Fwww.ti.com%252Ftool%252FEK-TM4C123GXL%253FkeyMatch%253DTIVA%2BC%2BSERIES%2BTM4C123G%2BLAUNCHPAD%2BEVALUATION%2BBOARD
	// GPIO Port E interrupts are "Interrupt Number" = 4
	SYSCTL_RCGCGPIO_R |= 0x10; // Port E
	__nop(); __nop(); __nop(); __nop();
	GPIO_PORTE_DIR_R &= ~(0x7); // Pins 0, 1, 2 are set as inputs, for each button
	GPIO_PORTE_DEN_R |= 0x7;
	// https://microcontrollerslab.com/gpio-interrupts-tm4c123-tiva-launchpad-edge-level-triggered/#Why_do_we_need_to_use_TM4C123_GPIO_Interrupts
	NVIC->ISER[0] |= (1 << 4); // Port E GPIO interrupts
	GPIOE->IM |= 0x2; // PE1 is our interrupt
	GPIOE->IS &= ~0x2; // PE1 is edge-triggered interrupt
	GPIOE->IEV |= 0x2; // PE1 is rising edge triggered interrupt
	NVIC->IP[30] = 1 << 5; // GPIO interrupt priority to 1
	
  ST7735_FillScreen(0x0000);            // set screen to black  
	
	// Setup custom variables as needed
	oldRow = keyRow = MIDDLE;
	EnableInterrupts();
	
	ST7735_DrawBitmap(0,159, keyboard, 128, 160);
	// Main loop
	while(1)
	{
		oldRow = keyRow;
		// Pin 2 - Go UP a keyboard row
		if ((GPIO_PORTE_DATA_R & 0x1) && keyRow != 0) {
			keyRow--; // since the enum datatype is technically numeric, we can decrement
		// i.e:
		// Middle -> Top
		// Bottom -> Middle
			SysTick_Wait(80000000/2); // half sec debounce
		}
		// Pin 0 - Go DOWN a keyboard row
		else if ((GPIO_PORTE_DATA_R & 0x4) && keyRow != 2) {
			keyRow++;
			SysTick_Wait(80000000/2); // half sec debounce
		}
		// check if we've changed spaces at all, if so we need to redraw
		if (OldIndex != KeyIndex || oldRow != keyRow) {
			ST7735_DrawBitmap(0, 159, keyboard, 128, 160);
		}
		DrawLetterOutline(keyRow, KeyIndex);
	}
}

// My tasks:
/*
		- Get a basic keyboard running on my LCD display
			a) void ST7735_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
		- 2 interrupts on my MC:
			a) SysTick (ADC sampling)
			b) GPIO (for when a button is pressed, to send the letter)
*/

// SysTick handler for sampling the ADC
void SysTick_Handler() {
	uint32_t adcVal = ADC_In();
	// Map ADC value to a keyboard value, depending on the current row we're on
	KeyIndex = ConvertADC(adcVal);
}
// GPIO Port E interrupt handler
// this triggers on PE1
void GPIOPortE_Handler() {
		// Send to UART
	// Sends 2 characters bc RX interrupt is on 1->2 elements
	switch (keyRow) {
		case TOP: {
			UART_OutChar('=');
			UART_OutChar(TopRow[KeyIndex]);
			break;
		}
		case MIDDLE: {
			UART_OutChar('=');
			UART_OutChar(MiddleRow[KeyIndex]);
			break;
		}
		case BOTTOM: {
			UART_OutChar('=');
			UART_OutChar(BottomRow[KeyIndex]);
			break;
		}
	}
	SysTick_Wait(80000000/10);
	GPIO_PORTE_ICR_R |= 0x2; // clear interrupt for PE1
}

// Turns the sampled ADC value into a valid character index
uint32_t ConvertADC(uint32_t adc_val) {
	/*
	Logic here:
			ADC value ranges from 0 to 4095 (or 4096, not really a difference here.)
			We want to scale down 0-4096 to between 0 and StringLen-1 of the current row we're on
			
	The constants were found by doing:
			4096/(StringLength+1 * x) = MaximumIndex, and solving for x
	*/
	uint32_t key = 0;
	switch (keyRow) {
		case TOP: {
			key = adc_val / (38 * StringLen(TopRow));
			break;
		}
		case MIDDLE: {
			key = adc_val / (50 * StringLen(MiddleRow));
			break;
		}
		case BOTTOM: {
			key = adc_val / (83 * StringLen(BottomRow));
			break;
		}
	}
	
	return key;
}

/*
		Draws a rectangle around the selected key
		A little hacky, and constants are measured from the BMP
*/
typedef struct Line {
	int16_t x, y, l, color;
} Line_t;
void DrawLetterOutline(enum KeyboardRow row, uint8_t index) {
	OldIndex = index;
	int16_t startX, startY;
	if (row == TOP) { startX = 103; startY = 4; }
	else if (row == MIDDLE) { startX = 76; startY = 12; }
	else { startX = 47; startY = 26; }
	
	const int16_t width = (122-103);
	const int16_t height = (18-3);
	
	Line_t horiz1 = {startX, startY + (height*index), width, ST7735_CYAN};
	Line_t horiz2 = {startX, startY + (height*(index+1)), width, ST7735_CYAN};
	Line_t vert1 = {startX, startY + (height*index), height, ST7735_CYAN};
	Line_t vert2 = {startX + width, startY + (height*index), height, ST7735_CYAN};

	ST7735_DrawFastHLine(horiz1.x, horiz1.y, horiz1.l, horiz1.color);
	ST7735_DrawFastHLine(horiz2.x, horiz2.y, horiz2.l, horiz2.color);
	ST7735_DrawFastVLine(vert1.x, vert1.y, vert1.l, vert1.color);
	ST7735_DrawFastVLine(vert2.x, vert2.y, vert2.l, vert2.color);
}

