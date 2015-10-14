#ifndef TIMER0_H
#define TIMER0_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include "PWM.h"

// *****************
// * timer0 Module *
// *****************
// The Timer0 module controls the update frequency of the PWM output.

// constants
#define FREQ_MULTIPLIER 2 // shift by two to accomodate subdivision
						  // quantization
#define WAVE_RAMP   0
#define WAVE_TRI    1
#define WAVE_SQUARE 2
#define WAVE_SINE   3
#define WAVE_LUMPS  4

#define NUM_WAVES   5

// variables
unsigned int* freqPtr;
unsigned int* wavePtr;
unsigned int  phaseAcc;
unsigned int  overflowCount;
unsigned int  shiftedOVF;
unsigned int  compareVal;
unsigned int  step;
unsigned int  waveNum;
unsigned int  rem;
unsigned int  idx;
int           diff;
int 		  interp;
int           curr;
int           next;

// function declarations
void init_timer(unsigned int* freqControl, 
	            unsigned int* waveSelect);
void en_timer_interrupt(void);

#endif