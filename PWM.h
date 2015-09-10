#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

// **************
// * PWM Module *
// **************
// The PWM module sources the analog output of the LFO. The output has a 
// resolution of 10 bits and a sampling frequency of 19.5 kHz (20 MHz/1024).

// function declarations
void init_PWM(void);
void write_PWM(unsigned int val);

#endif