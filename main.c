#include <avr/io.h>

#include "PWM.h"
#include "timer0.h"
#include "ADC.h"

int main(void) {
  // set clock prescale
  // CLKPCE   =    0b1: enable changes to CLKPS3:0
  // CLKPS3:0 = 0b0000: /1 prescaler
  CLKPR = (0b1<<CLKPCE) | (0b0000<<CLKPS0);

  // global interrupt enable
  sei();

  // initialize PWM on PA6
  init_PWM();

  // initialize timer
  init_timer();
  en_timer_interrupt();

  // initialize ADC
  init_ADC();

  // frequency pot reading
  unsigned int freq_pot;
  // waveform select switch reading
  unsigned int wave_pot;
  // subdivion select pot
  unsigned int sub_pot;

  // loop forever
  while(1) {
    // read ADC values
    freq_pot = read_ADC(FREQ_CV);
    wave_pot = read_ADC(WAVE_SELECT);
    sub_pot  = read_ADC(SUB_DIV);
    // DEBUG -- write PWM 
    write_PWM(freq_pot);
  }

  return(0);
}