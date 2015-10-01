#ifndef INPUT_CAPTURE_H
#define INPUT_CAPTURE_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer0.h"

// ************************
// * Input Capture Module *
// ************************
// The input capture module is responsible for obtaining 
// tap tempo input input 

// Constants
#define BOUNCE_TIME 40 // 40*1/305 = 130 ms, time to wait for bouncing
					   // experimentally determined, should add filter 
					   // to switch input, this is way too long
#define TAP_TIMEOUT    // time to wait until frequency control goes to 
					   // pot
#define STATE_POT   0   // freq is controlled by pot
#define STATE_TAP   1   // freq is controlled by tap

// variables
unsigned long  tapOverflowCount;
unsigned int*  statePtr;
unsigned long* tapTimePtr;

// function declarations
void init_in_cap(unsigned long* tapAddr, 
	             unsigned int*  stateAddr);
void en_in_cap_interrupt(void);

#endif