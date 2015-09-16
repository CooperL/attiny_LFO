#include <avr/io.h>

#include "PWM.h"
#include "timer0.h"
#include "ADC.h"

int main(void) {
  // variables
  unsigned int freqPot; // frequency pot reading
  unsigned int wavePot; // waveform select switch reading
  unsigned int subPot;  // subdivion select pot
  unsigned int phase;   // phase accumulator

  // set clock prescale
  // CLKPCE   =    0b1: enable changes to CLKPS3:0
  // CLKPS3:0 = 0b0000: /1 prescaler
  CLKPR = (0b1<<CLKPCE) | (0b0000<<CLKPS0);

  // global interrupt enable
  sei();

  // initialize PWM on PA6
  init_PWM();

  // initialize timer
  init_timer(&freqPot, &phase);

  // initialize ADC
  init_ADC();

  // loop forever
  while(1) {
    // read ADC values
    // freqPot = ADC_RES-1 + ADC_OFFSET - read_ADC(FREQ_CV);
    freqPot = read_ADC(FREQ_CV);
    wavePot = read_ADC(WAVE_SELECT);
    subPot  = read_ADC(SUB_DIV);

    // DEBUG -- set output compare time using freq pot
    // OCR0A = freq_pot>>2;
  }

  return(0);
}