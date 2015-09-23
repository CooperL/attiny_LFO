#include <avr/io.h>

#include "PWM.h"
#include "timer0.h"
#include "ADC.h"

// CONSTANTS

// FUNCTION DECLARATIONS
unsigned int calc_freq(unsigned int fPot, unsigned int sPot);

// MAIN
int main(void) {
  // variables
  unsigned int freqPot; // frequency pot reading
  unsigned int wavePot; // waveform select switch reading
  unsigned int subPot;  // subdivion select pot
  unsigned int freqCon; // frequency control value

  // set clock prescale
  // CLKPCE   =    0b1: enable changes to CLKPS3:0
  // CLKPS3:0 = 0b0000: /1 prescaler
  CLKPR = (0b1<<CLKPCE) | (0b0000<<CLKPS0);

  // global interrupt enable
  sei();

  // initialize PWM on PA6
  init_PWM();

  // initialize timer
  init_timer(&freqCon, &subPot);

  // initialize ADC
  init_ADC();

  // LOOP FOREVER
  while(1) {
    // READ ADC VALs
    // frequency control pot
    freqPot = read_ADC(FREQ_CV);
    // wave select pot
    wavePot = read_ADC(WAVE_SELECT);
    // subdivision select pot
    subPot  = read_ADC(SUB_DIV);

    // CALCULATE FREQUENCY CONTROL VALUE
    freqCon = calc_freq(freqPot, subPot);
  }

  return(0);
}

// FUNCTION DEFINITIONS

// calculates a frequency control value to be used in timer0

// function specific constants
#define SUB_DIV_SHIFT 8 // shift sub pot by value
#define MULT_4_SHIFT  2 // shift to mult/div by 4
#define MULT_2_SHIFT  1 // shift to mult/div by 4
#define QUARTER_SEL   0 // possible subdivision selection ID
#define EIGHT_SEL     1 // possible subdivision selection ID
#define DOT_EIGHT_SEL 2 // possible subdivision selection ID
#define SIXT_SEL      3 // possible subdivision selection ID

unsigned int calc_freq(unsigned int fPot, unsigned int sPot) {
  // reverse and scale scale, CW = high freq, CCW = low freq
  unsigned int fPotRev = (ADC_RES + ADC_OFFSET - fPot)>>MULT_2_SHIFT;
  // scale subdivision from 0 to 3 (divide by 64)
  unsigned int subDiv = sPot>>SUB_DIV_SHIFT;
  // quantize fPotRev to increments of 4
  unsigned int fQuant = (fPotRev>>MULT_4_SHIFT)<<MULT_4_SHIFT;
  // calculate freq control value from subdivision
  if(subDiv == EIGHT_SEL) {
    // selection is eighth note
    return fQuant>>MULT_2_SHIFT;
  }
  else if(subDiv == DOT_EIGHT_SEL) {
    // selection is dotted eighth note
    // DEBUG -- just eights for now
    return fQuant>>MULT_2_SHIFT;
  }
  else if(subDiv == SIXT_SEL) {
    // selection is sixteenth note
    return fQuant>>MULT_4_SHIFT;
  }
  else {
    // selection is quarter note
    return fQuant;
  }
}