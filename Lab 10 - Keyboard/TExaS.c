// TExaS.c
// Runs on TM4C123
// Periodic timer interrupt data collection
// PLL turned on at 80 MHz
// Implements Logic Analyzer on Port B, D, E, or F
//

// Jonathan Valvano. Daniel Valvano
// 11/2/2021 

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2021

 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include <stdint.h>
//#include <string.h>
#include "../inc/tm4c123gh6pm.h"
#include "TExaS.h"
#include "PLL.h"


// Timer5A periodic interrupt implements logic analyzer
// 0 for scope
void Scope(void){  // called 10k/sec
  UART0_DR_R = (ADC1_SSFIFO3_R>>4); // send ADC to TExaSdisplay
}
// 1 for PA7-2 logic analyzer A
#define PA72       (*((volatile uint32_t *)0x400043F0))
void LogicAnalyzerA(void){  // called 10k/sec
  UART0_DR_R = (PA72>>2)|0x80;   // send data to TExaSdisplay
}
// 2 for PB6-0 logic analyzer B
#define PB60       (*((volatile uint32_t *)0x400051FC))
void LogicAnalyzerB(void){  // called 10k/sec
  UART0_DR_R = PB60|0x80;   // send data to TExaSdisplay
}
// 3 for PC7-4 logic analyzer C
#define PC74       (*((volatile uint32_t *)0x400063C0))
void LogicAnalyzerC(void){  // called 10k/sec
  UART0_DR_R = (PC74>>4)|0x80;   // send data to TExaSdisplay
}
// 4 for PE5-0 logic analyzer E
void LogicAnalyzerE(void){  // called 10k/sec
  UART0_DR_R = GPIO_PORTE_DATA_R|0x80;   // send data to TExaSdisplay
}
// 4 for PF4-0 logic analyzer F
void LogicAnalyzerF(void){  // called 10k/sec
  UART0_DR_R = GPIO_PORTF_DATA_R|0x80;   // send data to TExaSdisplay
}

// ------------PeriodicTask2_Init------------
// Activate an interrupt to run a user task periodically.
// Give it a priority 0 to 6 with lower numbers
// signifying higher priority.  Equal priority is
// handled sequentially.
// Input:  task is a pointer to a user function
//         Bus clock frequency in Hz
//         freq is number of interrupts per second
//           1 Hz to 10 kHz
//         priority is a number 0 to 6
// Output: none
void (*PeriodicTask2)(void);   // user function
void PeriodicTask2_Init(void(*task)(void), 
  uint32_t busfrequency, uint32_t freq, uint8_t priority){//long sr;
    uint32_t volatile delay;
  if((freq == 0) || (freq > 10000)){
    return;                        // invalid input
  }
  if(priority > 6){
    priority = 6;
  }

  PeriodicTask2 = task;            // user function
  // ***************** Timer5 initialization *****************
  SYSCTL_RCGCTIMER_R |= 0x20;      // 0) activate clock for Timer5
 // while((SYSCTL_PRTIMER_R&0x20) == 0){};// allow time for clock to stabilize
    delay = SYSCTL_RCGCTIMER_R;
  TIMER5_CTL_R &= ~TIMER_CTL_TAEN; // 1) disable Timer5A during setup
                                   // 2) configure for 32-bit timer mode
  TIMER5_CFG_R = TIMER_CFG_32_BIT_TIMER;
                                   // 3) configure for periodic mode, default down-count settings
  TIMER5_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
                                   // 4) reload value
  TIMER5_TAILR_R = (busfrequency/freq - 1);
  TIMER5_TAPR_R = 0;               // 5) bus clock resolution
                                   // 6) clear TIMER5A timeout flag
  TIMER5_ICR_R = TIMER_ICR_TATOCINT;
  TIMER5_IMR_R |= TIMER_IMR_TATOIM;// 7) arm timeout interrupt
                                   // 8) priority
  NVIC_PRI23_R = (NVIC_PRI23_R&0xFFFFFF00)|(priority<<5);
// interrupts enabled in the main program after all devices initialized
// vector number 108, interrupt number 92
  NVIC_EN2_R = 1<<28;              // 9) enable IRQ 92 in NVIC
  TIMER5_CTL_R |= TIMER_CTL_TAEN;  // 10) enable Timer5A 32-b

}

void Timer5A_Handler(void){
  TIMER5_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER5A timeout
  (*PeriodicTask2)();               // execute user task
}

// ------------PeriodicTask2_Stop------------
// Deactivate the interrupt running a user task
// periodically.
// Input: none
// Output: none
void PeriodicTask2_Stop(void){
  if(SYSCTL_RCGCTIMER_R&0x20){
    // prevent hardware fault if PeriodicTask2_Init() has not been called
    TIMER5_ICR_R = TIMER_ICR_TATOCINT;// clear TIMER5A timeout flag
    NVIC_DIS2_R = 1<<28;           // disable IRQ 92 in NVIC
  }
}

#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable


//------------UART0_Init------------
// Busy-wait verion, 
// Initialize the UART for 115,200 baud rate (assuming busfrequency system clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
//        bus clock frequency in Hz
//        baud rate in bps
// Output: none
void UART0_Init(void){
  SYSCTL_RCGCUART_R |= 0x01; // activate UART0
  SYSCTL_RCGCGPIO_R |= 0x01; // activate port A
  UART0_CTL_R &= ~UART_CTL_UARTEN;      // disable UART

  UART0_IBRD_R = 43;                    // IBRD = int(80,000,000 / (16 * 115200)) = int(43.402778)
  UART0_FBRD_R = 26;                    // FBRD = round(0.402778 * 64) = 26
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART0_CTL_R |= UART_CTL_UARTEN;       // enable UART
  GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1-0
  GPIO_PORTA_DEN_R |= 0x03;             // enable digital I/O on PA1-0
                                        // configure PA1-0 as UART
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03;          // disable analog functionality on PA
}

//------------UART0_InChar------------
// Busy-wait verion, wait for new serial port input
// Input: none
// Output: ASCII code for key typed
char UART0_InChar(void){
  while((UART0_FR_R&UART_FR_RXFE) != 0);
  return((char)(UART0_DR_R&0xFF));
}

//------------UART0_OutChar------------
// Busy-wait verion, Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART0_OutChar(char data){
  while((UART0_FR_R&UART_FR_TXFF) != 0);
  UART0_DR_R = data;
}
//------------UART0_OutString------------
// Busy-wait verion, Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART0_OutString(char *pt){
  while(*pt){
    UART0_OutChar(*pt);
    pt++;
  }
}



// start conversions, sample always
// ADC1
// PD3 Ain4
// 16-point averaging 125kHz sampling
void ADC1_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGCADC_R |= 0x02;       // 1) ADC1 clock
  SYSCTL_RCGCGPIO_R |= 0x08;      // 2) activate clock for Port D
  while((SYSCTL_PRGPIO_R&0x08) == 0){};// allow time for clock to stabilize
  GPIO_PORTD_DIR_R &= ~0x08;      // 3) make PD3 input
  GPIO_PORTD_AFSEL_R |= 0x08;     // 4) enable alternate function on PD3
  GPIO_PORTD_DEN_R &= ~0x08;      // 5) disable digital I/O on PD3
  GPIO_PORTD_AMSEL_R |= 0x08;     // 6) enable analog functionality on PD3
  for(delay = 0; delay<20; delay++){};  // allow time for clock to stabilize
  ADC1_PC_R = 0x01;               // 7) 125K rate
  ADC1_SSPRI_R = 0x0123;          // 8) Sequencer 3 is highest priority
  ADC1_ACTSS_R = 0x0000;          // 9) disable sample sequencer 3
  ADC1_EMUX_R |= 0xF000;          // 10) seq3 is always/continuous trigger
  ADC1_SAC_R = 0x03;              //   8-point average 125kHz/8 = 15,625 Hz
  ADC1_SSMUX3_R = 4;              // 11) set channel 4
  ADC1_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC1_IM_R = 0x0000;             // 13) disable SS3 interrupts
  ADC1_ACTSS_R = 0x0008;          // 14) enable sample sequencer 3
}
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// ************TExaS_Init*****************
// Initialize grader, triggered by periodic timer
// Bus clock frequency will be 80 MHz
// This needs to be called once
// Inputs: Scope Logic analyzer or Grader
// Outputs: none
// 0 for TExaS oscilloscope on PD3, 
// 1 for PA7-2 logic analyzer A
// 2 for PB6-0 logic analyzer B
// 3 for PC7-4 logic analyzer C
// 4 for PE5-0 logic analyzer E
// 5 for Lab6 grader, 
// 6 for none
void TExaS_Init(enum TExaSmode mode){
	//int flag;
  DisableInterrupts();
  PLL_Init(Bus80MHz);      // 80  MHz\
	
 // UART0_Init();    // busy-wait initialize UART
 
  

  if(mode == LOGICANALYZERA){
  // enable 10k periodic interrupt if logic analyzer mode
   SYSCTL_RCGCGPIO_R |= 0x01; // port A needs to be on
   PeriodicTask2_Init(&LogicAnalyzerA,80000000,10000,5); // run logic analyzer
   EnableInterrupts();
  }
  if(mode == LOGICANALYZERB){
  // enable 10k periodic interrupt if logic analyzer mode
   SYSCTL_RCGCGPIO_R |= 0x02; // port B needs to be on
   PeriodicTask2_Init(&LogicAnalyzerB,80000000,10000,5); // run logic analyzer
   EnableInterrupts();
  }
  if(mode == LOGICANALYZERC){
  // enable 10k periodic interrupt if logic analyzer mode
   SYSCTL_RCGCGPIO_R |= 0x04; // port C needs to be on
   PeriodicTask2_Init(&LogicAnalyzerC,80000000,10000,5); // run logic analyzer
   EnableInterrupts();
  }
  if(mode == LOGICANALYZERE){
  // enable 10k periodic interrupt if logic analyzer mode
   SYSCTL_RCGCGPIO_R |= 0x10; // port E needs to be on
   PeriodicTask2_Init(&LogicAnalyzerE,80000000,10000,5); // run logic analyzer
   EnableInterrupts();
  }
  if(mode == LOGICANALYZERF){
  // enable 10k periodic interrupt if logic analyzer mode
   SYSCTL_RCGCGPIO_R |= 0x20; // port F needs to be on
   PeriodicTask2_Init(&LogicAnalyzerF,80000000,10000,5); // run logic analyzer
   EnableInterrupts();
  }
  if(mode == SCOPE){
    ADC1_Init();  // activate PD3 as analog input
    PeriodicTask2_Init(&Scope,80000000,10000,5); // run scope at 10k
    EnableInterrupts();
  }
}

// ************TExaS_Stop*****************
// Stop the transfer
// Inputs:  none
// Outputs: none
void TExaS_Stop(void){
  PeriodicTask2_Stop();
}


