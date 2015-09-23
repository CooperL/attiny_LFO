#ifndef ADC_H
#define ADC_H

#include <avr/io.h>

// **************
// * ADC Module *
// **************
// The ADC module is responsible from getting external input from 
// potentiometers.

// Constants
#define FREQ_CV     0b000000 // ADC channel selection bits
#define WAVE_SELECT 0b000001 // ADC channel selection bits
#define SUB_DIV     0b000010 // ADC channel selection bits
#define ADC_OFFSET  10       // added to ADC readings
#define ADC_RES     1024     // ADC resolution

// variables
unsigned int high;
unsigned int low;

// function declarations
void init_ADC(void);
unsigned int read_ADC(unsigned int ch);

#endif