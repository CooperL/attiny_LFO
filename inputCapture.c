#include "inputCapture.h"

// FUNCTION
// initialize input capture module
void init_in_cap(unsigned long* tapAddr, 
	             unsigned int* stateAddr) {
  // DATA DIRECTION REGISTER
  // DDRA3 = 0: PA3 is input
  DDRA &= ~(0b1<<PA3);

  // PORTA REGISTER
  // PORTA3 = 0: no pullup resistor
  PORTA &= ~(0b1<<PA3);

  // initialize tap overflow count
  tapOverflowCount = 0;

  // initialize tap time ptr
  tapTimePtr = tapAddr;

  // initialzie state ptr
  statePtr = stateAddr;

  // enable interrupt
  en_in_cap_interrupt();
}

// FUNCTION
// enable input capture interrupt
void en_in_cap_interrupt(void) {
	// MCU CONTROL REGISTER
	// ISC01:0 = 11: interrupt triggered on rising edge
	MCUCR = (0b11<<ISC00);

	// GENERAL INTERRUPT MASK REGISTER
	// PCIE0 = 1: enable pin change 0 interrupt
	GIMSK = (0b1<<PCIE0);

	// PIN CHANGE MASK REIGSTER 0
	// PCINT3 = 1: enable pinc change interrupt on PA3
	PCMSK0 = (0b1<<PCINT3);
}

// INPUT CAPTURE ISR
// input pin change interrupt
ISR(PCINT0_vect) {
	// check for bouncing
	if(tapOverflowCount >= BOUNCE_TIME) {
		// freq state is tap
		// *statePtr = STATE_TAP;

		// record time
		*tapTimePtr = tapOverflowCount;

		// reset phase accumulator
		phaseAcc = 0;

	    // reset tap overflow count
	    tapOverflowCount = 0;
	}
}