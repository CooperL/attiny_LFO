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
#define WAVE_SAW    1
#define WAVE_TRI    2
#define WAVE_SQUARE 3
#define WAVE_SINE   4
#define WAVE_SWEEP  5
#define WAVE_SPIKE  7

// variables
unsigned int* freqPtr;
unsigned int* wavePtr;
unsigned int  phaseAcc;
unsigned int  overflowCount;
unsigned int  shiftedOVF;
unsigned int  compareVal;
unsigned int  step;
unsigned int  waveNum;
unsigned int  rem1;
unsigned int  rem2;
unsigned int  idx1;
unsigned int  idx2;
int           diff;
int 		  interp;

// function declarations
void init_timer(unsigned int* freqControl, 
	            unsigned int* waveSelect);
void en_timer_interrupt(void);

#endif