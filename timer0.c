#include "timer0.h"

// initialize timer0 module
void init_timer(void) {
  // init timer0
  
  // TIMER/COUNTER CONTROL REGISTERS
  // COM0A1:0 = 00: normal port operation, OC0A disconnected
  // WGM01:0  = 00: normal mode
  TCCR0A = (0b00<<COM0A0) | (0b00<<WGM00);
  // WGM02 = 0: normal mode
  // CS02:0  = 100: IO clock, /256 prescaler (20 MHz/256/256 = 305 Hz)
  TCCR0B = (0b0<<WGM02) | (0b100<<CS00);
  
  // DATA DIRECTION REGISTERS
  // DDRA0 = 1: PA0 is output DEBUG
  DDRA = DDRA | (0b1<<PA0);
  // DDRB2 = 1: PB2 is output (OC0A) DEBUG
  DDRB = (0b1<<PB2);
}

// enable timer0 interrupt
void en_timer_interrupt(void) {
  // enable interrupt
  
  // TIMER INTERRUPT MASK REGISTER
  // TOIE0 = 0b1: enable timer0 overflow interrupt
  TIMSK0 = TIMSK0 | (0b1<<TOIE0);
}

// need to take out definition of ISR(TIM0_OVF_vect) in wiring.c and 
// remove Stream.cpp from Java/hardware/arduino/avr/cores/arduino
ISR(TIM0_OVF_vect) {
  PORTA = (PORTA & 0b1)^0b1;
}