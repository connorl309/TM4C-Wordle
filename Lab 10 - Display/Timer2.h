#ifndef TIMER_2_H
#define TIMER_2_H

#include <stdint.h>

#include "../inc/tm4c123gh6pm.h"


// ***************** Timer1_Init ****************
// Activate TIMER1 interrupts to run user task periodically
// Inputs:  period in units (1/clockfreq)
//          priority is 0 (high) to 7 (low)
// Outputs: none
void Timer2_Init(uint32_t period, uint32_t priority);
#endif