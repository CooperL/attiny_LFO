#include "timer0.h"

// FUNCTION
// initialize timer0 module
void init_timer(unsigned int* fAddr, unsigned int* subAddr) {
  // TIMER/COUNTER CONTROL REGISTERS
  // COM0A1:0 = 00: normal port operation, OC0A disconnected
  // WGM01:0  = 00: normal mode
  TCCR0A = (0b00<<COM0A0) | (0b00<<WGM00);
  // WGM02 = 0: normal mode
  // CS02:0  = 100: IO clock, /256 prescaler (20 MHz/256/256 = 305 Hz)
  TCCR0B = (0b0<<WGM02) | (0b100<<CS00);
  
  // DATA DIRECTION REGISTER B
  // DEBUG -- DDRB2 = 1: PB2 is output to confirm interrupt timing
  DDRB |= (0b1<<PB2);

  // initialize frequency control address
  freqPtr = fAddr;
  // initialize subdivision select address
  subSelect = subAddr;
  // initialize phase accumulator
  phaseAcc = 0;
  // initialize overflow count
  overflowCount = 0;

  // enable timer interrupt
  en_timer_interrupt();
}

// FUNCTION
// enable timer0 interrupt
void en_timer_interrupt(void) {
  // TIMER INTERRUPT MASK REGISTER
  // OCIE0A = 0b1: enable timer0 output compare interrupt
  // TOIE0  = 0b1: enable timer0 overflow interrupt
  TIMSK0 |= (0b1<<OCIE0A) | (0b1<<TOIE0);

  // OUTPUT COMPARE REGISTER A
  // initialize output compare value to 0
  OCR0A = 0;
}

// OUTPUT COMPARE ISR
// Output sampling interrupt
ISR(TIM0_COMPA_vect) {
  // dereference pointer
  step = *freqPtr;
  // adjust overflow to add to output compare register
  shiftedOVF = overflowCount<<8;
  // calculate output compare value
  compareVal = shiftedOVF + OCR0A;
  // check if adjusted output compare is met
  if(compareVal >= step) {
    // reset overflow count
    overflowCount = 0;
    // update output compare register
    OCR0A = (OCR0A + step)%0x100; 
    // increment phase
    phaseAcc = (phaseAcc + 1)%PWM_RES;
    // write PWM
    OCR1A = phaseAcc;
  }
}

// OVERFLOW ISR
// timer overflow COUNTER
ISR(TIM0_OVF_vect) {
  // increment overflow counter
  overflowCount++;
}
