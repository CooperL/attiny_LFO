#include "timer0.h"
#include "inputCapture.h"
#include "wavetable.h"

// FUNCTION
// initialize timer0 module
void init_timer(unsigned int* fAddr, 
                unsigned int* wAddr) {
  // TIMER/COUNTER CONTROL REGISTERS
  // COM0A1:0 = 00: normal port operation, OC0A disconnected
  // WGM01:0  = 00: normal mode
  TCCR0A = (0b00<<COM0A0) | (0b00<<WGM00);
  // WGM02 = 0: normal mode
  // CS02:0  = 100: IO clock, /256 prescaler (20 MHz/256/256 = 305 Hz)
  TCCR0B = (0b0<<WGM02) | (0b100<<CS00);

  // initialize frequency control address
  freqPtr = fAddr;

  // initialize wave select address
  wavePtr = wAddr;

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
    // get wave number
    waveNum = *wavePtr;
    // write PWM based on waveform
    if(waveNum == WAVE_RAMP) {
      OCR1A = phaseAcc;
    }
    else if(waveNum == WAVE_SAW) {
      OCR1A = PWM_RES - phaseAcc;
    } 
    else if(waveNum == WAVE_TRI) {
      if(phaseAcc < PWM_RES>>1) {
        OCR1A = 2*phaseAcc;
      }
      else {
        OCR1A = PWM_RES - (2*(phaseAcc - (PWM_RES>>1)));
      }
    } 
    else if(waveNum == WAVE_SQUARE) {
      if(phaseAcc < PWM_RES>>1) {
        OCR1A = 0;
      }
      else {
        OCR1A = PWM_RES - 1;
      }
    } 
    // we need to use a wave table
    else {
      // calculate remainder
      rem1 = PWM_RES%TABLE_LEN;
      rem2 = PWM_RES%SHORT_TABLE_LEN;
      idx1 = phaseAcc>>2;
      idx2 = phaseAcc>>3;
      if(waveNum == WAVE_SINE) {
        diff = (int) sine[idx1] - (int) sine[(idx1 + 1)%TABLE_LEN];
        interp = rem1*diff;
        OCR1A = sine[idx1]<<2 + interp;
      } 
      else if(waveNum == WAVE_SWEEP) {
        diff = (int) spike[idx2] - (int) spike[(idx2 + 1)%SHORT_TABLE_LEN];
        interp = rem2*diff;
        OCR1A = (spike[idx2]<<2) + interp;
      } 
      else if(waveNum == WAVE_SPIKE) {
        diff = (int) spike[idx2] - (int) spike[(idx2 + 1)%SHORT_TABLE_LEN];
        interp = rem2*diff;
        OCR1A = (spike[idx2]<<2) + interp;
      }
    }
  }
}

// OVERFLOW ISR
// timer overflow COUNTER
ISR(TIM0_OVF_vect) {
  // increment overflow counter
  overflowCount++;
  // increment tempo overflow counter
  tapOverflowCount++;
}
