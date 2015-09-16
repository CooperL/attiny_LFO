#include <avr/io.h>

#include "PWM.h"
#include "timer0.h"
#include "ADC.h"

int main(void) {
  // variables
  unsigned int freqPot; // frequency pot reading
  unsigned int wavePot; // waveform select switch reading
  unsigned int subPot;  // subdivion select pot

  // set clock prescale
  // CLKPCE   =    0b1: enable changes to CLKPS3:0
  // CLKPS3:0 = 0b0000: /1 prescaler
  CLKPR = (0b1<<CLKPCE) | (0b0000<<CLKPS0);

  // global interrupt enable
  sei();

  // initialize PWM on PA6
  init_PWM();

  // initialize timer
  init_timer(&freqPot, &subPot);

  // initialize ADC
  init_ADC();

  // loop forever
  while(1) {
    // read ADC values

    // frequency control pot
    // reverse scale
    unsigned int freqPotRev = ADC_RES + ADC_OFFSET - read_ADC(FREQ_CV);
    // divide by 4 (0-255)
    freqPot = freqPotRev>>2;
    wavePot = read_ADC(WAVE_SELECT);

    // subdivision select pot
    // divide by 64
    subPot  = read_ADC(SUB_DIV)>>6;

    // DEBUG -- set output compare time using freq pot
    // OCR0A = freq_pot>>2;
  }

  return(0);
}