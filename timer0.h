#ifndef TIMER0_H
#define TIMER0_H

#include <avr/io.h>
#include <avr/interrupt.h>

// *****************
// * timer0 Module *
// *****************
// The Timer0 module controls the update frequency of the PWM output.

// variables
unsigned int* freqControl;
unsigned int* phaseAcc;

// function declarations
void init_timer(unsigned int* freqControl, unsigned int* phaseAcc);
void en_timer_interrupt(void);

#endif