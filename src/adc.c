#include "adc.h"

unsigned int getInnerAdcValue(unsigned char adcNumberOffset)
{
	ADMUX = (MUX_VALUE_OF_ADC+adcNumberOffset);
	ADCSRA &= (~(1<<ADIF));
	ADCSRA |= (1<<ADSC);//start to convert analog value to digital value using adc converter in atmega128.
	while((ADCSRA&(1<<ADIF)) == 0){};//waiting adc working

	ADCSRA &= (~(1<<ADIF));
	ADCSRA |= (1<<ADSC);//start to convert analog value to digital value using adc converter in atmega128.

	while((ADCSRA&(1<<ADIF)) == 0){};//waiting adc working

	return ADC;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initInnerAdcConverter()
{
	ADCSRA =((1<<ADEN)|(1<<ADFR)|ADC_PRESCALER_128);//setting ADC control register.

	getInnerAdcValue(7);
}
