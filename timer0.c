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
    else if(waveNum == WAVE_TRI) {
      if(phaseAcc < PWM_RES>>1) {
        OCR1A = phaseAcc<<1;
      }
      else {
        OCR1A = PWM_RES - ((phaseAcc - (PWM_RES>>1))<<1) - 1;
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
      rem = phaseAcc%4;
      // scale index from 0-255
      idx = phaseAcc>>2;
      if(waveNum == WAVE_LUMPS) {
        // calculate difference between next wave val and current
        diff = (int) lumps[(idx + 1)%TABLE_LEN] - (int) lumps[idx];
        // calculate interpolation factor
        interp = rem*diff;
        // add interpolation factor
        OCR1A = (lumps[idx]<<2) + interp;
      } 
      else if(waveNum == WAVE_SINE) {
        // get current wave val
        curr = lumps[(idx<<1)%TABLE_LEN]<<1;
        // get next wave val
        next = lumps[((idx + 1)<<1)%TABLE_LEN]<<1;
        // check if it should be negative
        if(idx == TABLE_LEN) {
          curr = -curr;
        }
        else if(idx > TABLE_LEN>>1) {
          curr = -curr;
          next = -next;
        }
        // calculate difference between next wave val and current
        diff = (int) next - (int) curr;
        interp = rem*diff;
        OCR1A = curr + interp + (PWM_RES>>1) - 1;
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
