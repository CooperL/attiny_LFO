#include <avr/io.h>

#include "PWM.h"
#include "timer0.h"

int main(void) {
  // set clock prescale
  //   CLKPCE =    0b1: enable changes to CLKPS3:0
  // CLKPS3:0 = 0b0000: /1 prescaler
  CLKPR = (0b1<<CLKPCE) | (0b0000<<CLKPS0);

  // global interrupt enable
  sei();
  
  // initialize PWM on PA6
  init_PWM();

  // initialize timer
  init_timer();
  en_timer_interrupt();

  // loop forever
  for(;;) {
    write_PWM((OCR1A+1)%0x3FF);
  }

  return(0);
}