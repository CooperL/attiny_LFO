#include "ADC.h"

// FUNCTION
// initialize ADC module
void init_ADC(void) {
  // ADC MUX SELECTION REGISTER
  // REFS1:0 =     00: Vcc used as reference
  // MUX5:0  = 000000: initialize to channel 0
  ADMUX = (0b00<<REFS0) | (0b000000<<MUX0);

  // ADC CONTROL AND STATUS REGISTER A
  // ADEN    =   1: enable ADC
  // ADSC    =   0: do not start conversion
  // ADATE   =   0: do not enable auto trigger mode
  // ADIE    =   0: do not enable ADC interrupt
  // ADPS2:0 = 110: divide clock by 64
  ADCSRA = (0b1<<ADEN) | (0b0<<ADSC) | (0b0<<ADATE) | (0b0<<ADIE) | (0b110<<ADPS0);
  
  // ADC CONTROL AND STATUS REGISTER B
  // BIN   = 0: unipolar mode
  // ADLAR = 0: right adjust
  ADCSRB = (0b0<<BIN) | (0b0<<ADLAR);

  // DIGITAL INPUT DISABLE
  // DIDR0:2 = 1: turn off digital input on these pins
  DIDR0 = 0b111;
}

// FUNCTION
// read ADC channel
unsigned int read_ADC(unsigned int ch) {
  // turn off ADC
  ADCSRA &= ~(0b1<<ADEN);

  // select channel from ADMUX register
  ADMUX = (ADMUX & 0b11000000) | (ch<<MUX0);

  // turn on ADC u fuck
  ADCSRA |= (0b1<<ADEN);

  // start ADC conversion
  ADCSRA |= (0b1<<ADSC);

  // wait for conversion to finish
  // ADSC is set to 1 while converting, 0 when finished
  while(ADCSRA & (0b1<<ADSC));

  // read ADC data registers
  low = ADCL;
  
  high = ADCH;
  unsigned int val = (high<<8) + low;

  // prevent 0 ADC reading
  if (val < ADC_OFFSET) {
    val = ADC_OFFSET;
  }

  // return
  return val;
}