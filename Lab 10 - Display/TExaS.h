// TExaS.h
// Runs on TM4C123
// Periodic timer Timer4A, ADC1, PD3, and UART0 used to create the scope
// Periodic timer Timer5A which will interact with debugger and grade the lab 
// It initializes on reset and runs whenever interrupts are enabled
// Jonathan Valvano
// 1/12/2022 

/* This example accompanies the book
   "Embedded Systems: Real Time Operating Systems for ARM Cortex M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2018
   Section 6.4.5, Program 6.1

 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
#ifndef TEXAS_H
#define TEXAS_H

// 0 for TExaS oscilloscope on PD3, 
// 1 for PA7-2 logic analyzer A
// 2 for PB6-0 logic analyzer B
// 3 for PC7-4 logic analyzer C
// 4 for PE5-0 logic analyzer E
// 5 for PF4-0 logic analyzer F
enum TExaSmode{
  SCOPE,
  LOGICANALYZERA,
  LOGICANALYZERB,
  LOGICANALYZERC,
  LOGICANALYZERE,
  LOGICANALYZERF,
	NONE
};

// ************TExaS_Init*****************
// Initialize scope or logic analyzer, triggered by periodic timer
// This needs to be called once
// Inputs: Scope or Logic analyzer or Grader or None
// Outputs: none
void TExaS_Init(enum TExaSmode mode);

// ************TExaS_Stop*****************
// Stop the transfer
// Inputs:  none
// Outputs: none
void TExaS_Stop(void);
#endif

