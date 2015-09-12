#include "ADC.h"

// initialize ADC module
void init_ADC(void) {
  // init ADC
  
  // ADC MUX SELECTION REGISTER
  // REFS1:0 =     00: Vcc used as reference
  // MUX5:0  = 000000: initialize to channel 0
  ADMUX = (0b00<<REFS0) | (0b000000<<MUX0);

  // ADC CONTROL AND STATUS REGISTER A
  // ADEN    =   1: enable ADC
  // ADSC    =   0: do not start conversion
  // ADATE   =   0: do not enable auto trigger mode
  // ADIE    =   0: do not enable ADC interrupt
  // ADPS2:0 = 111: divide clock by 128
  ADCSRA = (0b1<<ADEN) | (0b0<<ADSC) | (0b0<<ADATE) | (0b0<<ADIE) | (0b111<<ADPS0);
  
  // ADC CONTROL AND STATUS REGISTER B
  // BIN   = 0: unipolar mode
  // ADLAR = 0: right adjust
  ADCSRB = (0b0<<BIN) | (0b0<<ADLAR);
}

// read ADC channel
unsigned int read_ADC(unsigned int ch) {
  // turn off ADC
  ADCSRA = ADCSRA & ~(0b1<<ADEN);
  // select channel from ADMUX register
  ADMUX = (ADMUX & 0b11000000) | (ch<<MUX0);
  // turn on ADC
  ADCSRA = ADCSRA | (0b1<<ADEN);
  // start ADC conversion
  ADCSRA = ADCSRA | (0b1<<ADSC);
  // wait for conversion to finish
  // ADIF is set to 1 when conversion complete, 0 while converting
  while(ADCSRA & (0b1<<ADSC));
  // read ADC data registers
  unsigned int low = ADCL;
  unsigned int high = ADCH;
  unsigned int val = (high<<8) | low;
  // return
  return val;
}