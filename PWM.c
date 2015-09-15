#include "PWM.h"

// FUNCTION
// initialize PWM module
// PWM output on PA6
void init_PWM(void) {
  // TIMER/COUNTER CONTROL REGISTERS
  // COM1A1:0 = 10: clear OC1A on compare match, set OC1A at bottom 
  // WGM11:10 = 11: fast PWM, 10-bit (WGM13:10 = 0111)
  TCCR1A = (0b10<<COM1A0) | (0b11<<WGM10);
  // WGM13:12 =  01: fast PWM, 10-bit (WGM13:10 = 0111)
  // CS12:10  = 001: IO clock, no prescaling (20 MHz)
  TCCR1B = (0b01<<WGM12) | (0b001<<CS10);
  
  // DATA DIRECTION REGISTER
  // DDRA6 = 1: PA6 is output (OC1A) 
  DDRA |= (0b1<<PA6);
  
  // OUTPUT COMPARE REGISTER
  // OCR1A9:0 = 0x1FF: initialize to 50% duty cycle, full scale = 0x3FF
  OCR1A = 0x1FF; 
}

// FUNCTION
// writes analog val to PWM output
// max val is 0x3FF
void write_PWM(unsigned int val) {
  OCR1A = val;
}