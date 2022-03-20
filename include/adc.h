#ifndef _ADC_H_
#define _ADC_H_

//Define ADC register
//ADMUX
#define	REFS1	7//reference voltage
#define	REFS0	6//reference voltage
#define	ADLAR	5//stand in a row, 0 is right, 1 is left
#define	MUX4	4//select ADC, reading datasheet
#define	MUX3	3
#define	MUX2	2
#define	MUX1	1
#define	MUX0	0

//ADCSRA
#define	ADEN	7//ADC circuit Enable
#define	ADSC	6//ADC Translation is Enable(Now Processing Analog to Digital?) , 1 is busy, 0 is idle.
#define	ADFR	5//Continue Analog value translate to digital value? 1 is continuous, 0 is translate 1 times.
#define	ADIF	4//ADC Interrupe falg, when ADC converting is done, set 1. when Interrup is occured, reset 0.
#define	ADIE	3//If programmer set register that is SREG and ADC converting is done that is ADIF register is set that is 1, ADC converting Interrup is started.
#define	ADPS2	2//ADC clock frequency setting
#define	ADPS1	1
#define	ADPS0	0//2^(XXX), ex) 2^(111) is mean, ADPS register is set ADPS2 = 1, ADPS1=1, ADPS0 = 1
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ADC_PRESCALER			0x00					//prescaler is 2

#define ADC_PRESCALER_2		(ADC_PRESCALER+1)		//prescaler is 2
#define ADC_PRESCALER_4		(ADC_PRESCALER_2+1)	//prescaler is 4
#define ADC_PRESCALER_8		(ADC_PRESCALER_4+1)	//prescaler is 8
#define ADC_PRESCALER_16	(ADC_PRESCALER_8+1)	//prescaler is 16
#define ADC_PRESCALER_32	(ADC_PRESCALER_16+1)	//prescaler is 32
#define ADC_PRESCALER_64	(ADC_PRESCALER_32+1)	//prescaler is 64
#define ADC_PRESCALER_128	(ADC_PRESCALER_64+1)	//prescaler is 128
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_ADC 				(F_CPU/(1<<ADC_PRESCALER_128))
const unsigned int ADC_CLOCK_PERIOD = ((int)(1000000UL/F_ADC));//ADC_CLOCK_PERIOD unit is micro seconds.
/* Using offset value of adc pin number */
#define	MUX_VALUE_OF_ADC	0xC0

#define	MUX_VALUE_OF_ADC0	(MUX_VALUE_OF_ADC+0)
#define	MUX_VALUE_OF_ADC1	(MUX_VALUE_OF_ADC+1)
#define	MUX_VALUE_OF_ADC2	(MUX_VALUE_OF_ADC+2)
#define	MUX_VALUE_OF_ADC3	(MUX_VALUE_OF_ADC+3)
#define	MUX_VALUE_OF_ADC4	(MUX_VALUE_OF_ADC+4)
#define	MUX_VALUE_OF_ADC5	(MUX_VALUE_OF_ADC+5)
#define	MUX_VALUE_OF_ADC6	(MUX_VALUE_OF_ADC+6)
#define	MUX_VALUE_OF_ADC7	(MUX_VALUE_OF_ADC+7)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
