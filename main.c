#include <avr/io.h>

#include "constants.h"
#include "PWM.h"
#include "timer0.h"
#include "ADC.h"
#include "inputCapture.h"

// CONSTANTS

// SHIFT CONSTANTS
#define MULT_4096_SHIFT  12  // shift to mult/div by 4096
#define MULT_1024_SHIFT  10  // shift to mult/div by 1024
#define MULT_128_SHIFT   7   // shift to mult/div by 128
#define MULT_4_SHIFT     2   // shift to mult/div by 4
#define MULT_2_SHIFT     1   // shift to mult/div by 2

// DIVIDERS
#define WAVE_DIV         205 // value to divide wav pot reading by
#define TAP_DIV          2   // value to divide tap val by
#define SUB_DIV_SHIFT    8   // shift sub pot by value

// SUB DIVISIONS
#define QUARTER_SEL      0  // possible subdivision selection ID
#define EIGHT_SEL        1  // possible subdivision selection ID
#define DOT_EIGHT_SEL    2  // possible subdivision selection ID
#define SIXT_SEL         3  // possible subdivision selection ID

// OTHER
#define SWELL_DEC        1    // amount to decrease freqCon each 
                              // swell sample period
#define SWELL_PER        100  // number of loops to wait to update
                              // freqCon in swell state, 
                              // experimentally determined

// FUNCTION DECLARATIONS
unsigned int calc_freq(unsigned int  fPot,
                       unsigned int  sPot,
                       unsigned long tVal,
                       unsigned long prevtVal,
                       unsigned int  state,
                       unsigned int  prevState,
                       unsigned int  fCon,
                       unsigned int  count);
void updateState(unsigned int  new,
                 unsigned int* statePtr,
                 unsigned int* prevPtr);

// MAIN
int main(void) {
  // variables
  unsigned int  freqPot;       // frequency pot reading
  unsigned int  wavePot;       // waveform select switch reading
  unsigned int  subPot;        // subdivion select pot
  unsigned int  freqCon;       // frequency control value
  unsigned long tapVal;        // tap tempo counter
  unsigned long prevTapVal;    // tap val before state -> swell
  unsigned int  waveSel;       // wave selection value
  unsigned int  freqState;     // variable that says whether freq is 
                               // controlled by pot or tap or swell
  unsigned int  prevFreqState; // previous freqState
  unsigned int  loopCount;     // counts number of loops, mod by SWELL_PER
  unsigned int  pressed;       // 1 while button is pressed, 0 otherwise

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
  pressed = 0;
  init_in_cap(&tapVal, &freqState, &pressed);

  // initialize freq con state
  prevFreqState = STATE_POT;
  freqState     = STATE_POT;

  // initialize loop counter
  loopCount = 0;

  // DEBUG -- set output to test swell
  DDRB = (PB2<<0b1);
  PORTB &= ~(PB2<<0b1);

  // LOOP FOREVER
  while(1) {
    // READ ADC VALs
    // frequency control pot
    freqPot = read_ADC(FREQ_CV);
    // wave select pot
    wavePot = read_ADC(WAVE_SELECT);
    // subdivision select pot
    subPot  = read_ADC(SUB_DIV);

    // STATE SWITCHING

    // SWELL
    // if tap is held down, switch to swell
    if((PINA & (0b1<<PA3)) 
       && (tapVal >= SWELL_TIME)) {
      pressed = 0;
      // capture tap tempo
      prevTapVal = tapVal;
      updateState(STATE_SWELL, &freqState, &prevFreqState);
    }
    // COMING OUT OF SWELL
    else if(~(PINA & (0b1<<PA3)) 
            && freqState == STATE_SWELL) {
      pressed = 0;
      updateState(prevFreqState, &freqState, &prevFreqState);
    }
    // TAP
    // the the button is released and state was not swell
    else if(~(PINA & (0b1<<PA3)) 
            && pressed 
            && freqState != STATE_SWELL) {
      pressed = 0;
      updateState(STATE_TAP, &freqState, &prevFreqState);
    }
    // POT
    // switch to pot control if pot rolled back
    else if(freqPot == ADC_OFFSET) {
      updateState(STATE_POT, &freqState, &prevFreqState);
    }
    // DEFAULT
    // else {
    //   freqState = prevFreqState;
    //   // updateState(prevFreqState, &freqState, &prevFreqState);
    // }

    // CALCULATE FREQUENCY CONTROL VALUE
    freqCon = calc_freq(freqPot, 
                        subPot, 
                        tapVal, 
                        prevTapVal,
                        freqState, 
                        prevFreqState,
                        freqCon,
                        loopCount);

    // CALCULATE WAVE SELECTION
    // divide by WAVE_DIV to scale from 0-4
    waveSel = wavePot/WAVE_DIV;

    // UPDATE LOOP COUNTER
    loopCount++;
    if(loopCount > SWELL_PER) {
      loopCount = 0;
    }
  }

  return(0);
}

// FUNCTION DEFINITIONS

// FUNCTION
// calculates a frequency control value to be used in timer0

unsigned int calc_freq(unsigned int  fPot, 
                       unsigned int  sPot, 
                       unsigned long tVal,
                       unsigned long prevtVal,
                       unsigned int  state,
                       unsigned int  prevState,
                       unsigned int  fCon,
                       unsigned int  count) {
  unsigned long freq;
  // check to see if pot, tap tempo, or swell is controlling freq
  if(state == STATE_TAP) {
    freq = tVal>>TAP_DIV;
  } 
  else if(state == STATE_SWELL) {
    if(count >= (SWELL_PER-1)) {
      if(fCon <= ADC_OFFSET) {
        // PORTB &= ~(PB2<<0b1);        
        freq = fCon;
      }
      else {
        // PORTB |= (PB2<<0b1);
        freq = fCon - SWELL_DEC;
      }
    }
  }
  else if(state == STATE_POT) {
    // reverse and scale scale, CW = high freq, CCW = low freq
    freq = (unsigned long) (ADC_RES + ADC_OFFSET - fPot)>>MULT_2_SHIFT;
  }
  // default to whatever freqCon already is
  else {
    freq = fCon;
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

// FUNCTION
// updates state variable and previous state variable only if state change

void updateState(unsigned int  new,
                 unsigned int* statePtr,
                 unsigned int* prevPtr) {
  unsigned int current = *statePtr;
  if(new != current) {
    *prevPtr = current;
    *statePtr = new;
  }
}






