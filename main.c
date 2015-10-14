#include <avr/io.h>

#include "PWM.h"
#include "timer0.h"
#include "ADC.h"
#include "inputCapture.h"

// CONSTANTS
#define MULT_4096_SHIFT  12  // shift to mult/div by 4096
#define MULT_1024_SHIFT  10  // shift to mult/div by 1024
#define MULT_128_SHIFT   7   // shift to mult/div by 128
#define MULT_4_SHIFT     2   // shift to mult/div by 4
#define MULT_2_SHIFT     1   // shift to mult/div by 2
#define WAVE_DIV         204 // value to divide wav pot reading by
#define TAP_DIV          2   // value to divide tap val by
#define SUB_DIV_SHIFT    8   // shift sub pot by value
#define CHANGE_PAD       2   // update freq state if freq pot has 
                             // changed by more than this number
#define BUFF_SIZE        4   // size of freq pot buffer

// FUNCTION DECLARATIONS
unsigned int calc_freq(unsigned int  fPot, 
                       unsigned int  sPot,
                       unsigned long tap,
                       unsigned int  state);
unsigned int checkFreqPotChange(unsigned int curr, 
                                unsigned int* buffer, 
                                unsigned int* idxPtr);

// MAIN
int main(void) {
  // variables
  unsigned int  freqPot;     // frequency pot reading
  unsigned int  wavePot;     // waveform select switch reading
  unsigned int  subPot;      // subdivion select pot
  unsigned int  freqCon;     // frequency control value
  unsigned long tapVal;      // tap tempo counter
  unsigned int  waveSel;     // wave selection value
  unsigned int  freqState;   // variable that says whether freq is 
                             // controlled by pot or tap

  // set clock prescale
  // CLKPCE   =    0b1: enable changes to CLKPS3:0
  // CLKPS3:0 = 0b0000: /1 prescaler
  CLKPR = (0b1<<CLKPCE) | (0b0000<<CLKPS0);

  // global interrupt enable
  sei();

  // initialize PWM on PA6
  init_PWM();

  // initialize timer
  init_timer(&freqCon, &waveSel);

  // initialize ADC
  init_ADC();

  // initialize input capture
  init_in_cap(&tapVal, &freqState);

  // initialize freq con state
  freqState = STATE_POT;

  // LOOP FOREVER
  while(1) {
    // READ ADC VALs
    // frequency control pot
    freqPot = read_ADC(FREQ_CV);
    // wave select pot
    wavePot = read_ADC(WAVE_SELECT);
    // subdivision select pot
    subPot  = read_ADC(SUB_DIV);

    // switch to pot control if pot rolled back
    if(freqPot == ADC_OFFSET) {
      freqState = STATE_POT;
    }

    // CALCULATE FREQUENCY CONTROL VALUE
    freqCon = calc_freq(freqPot, subPot, tapVal, freqState);

    // CALCULATE WAVE SELECTION
    // divide by WAVE_DIV to scale from 0-4
    waveSel = wavePot/WAVE_DIV;
  }

  return(0);
}

// FUNCTION DEFINITIONS

// FUNCTION
// calculates a frequency control value to be used in timer0

// function specific constants
#define QUARTER_SEL      0  // possible subdivision selection ID
#define EIGHT_SEL        1  // possible subdivision selection ID
#define DOT_EIGHT_SEL    2  // possible subdivision selection ID
#define SIXT_SEL         3  // possible subdivision selection ID

unsigned int calc_freq(unsigned int  fPot, 
                       unsigned int  sPot, 
                       unsigned long tapVal,
                       unsigned int  state) {
  unsigned long freq;
  // check to see if pot or tap tempo is controlling freq
  if(state == STATE_TAP) {
    freq = tapVal>>TAP_DIV;
  } 
  else if(state == STATE_POT) {
    // reverse and scale scale, CW = high freq, CCW = low freq
    freq = (unsigned long) (ADC_RES + ADC_OFFSET - fPot)>>MULT_2_SHIFT;
  }

  // scale subdivision from 0 to 3 (divide by 64)
  unsigned int subDiv = sPot>>SUB_DIV_SHIFT;

  // quantize fPotRev to increments of 4
  unsigned int fQuant = (freq>>MULT_4_SHIFT)<<MULT_4_SHIFT;

  // calculate freq control value from subdivision
  if(subDiv == EIGHT_SEL) {
    // selection is eighth note
    return fQuant>>MULT_2_SHIFT;
  }
  else if(subDiv == DOT_EIGHT_SEL) {
    unsigned int fQuantX3 = fQuant*3;
    return fQuantX3>>MULT_4_SHIFT;
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








