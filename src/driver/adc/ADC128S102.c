#include "ADC128S102.h"

///////////////////////////// ADC128S //////////////////////////////////
unsigned char SPI_ADC_Init()
{
	SPI_ADC_SPCR_SET;
	SPI_ADC_SPSR_SET;
	return 0;
}
/*
	description of getAdcValue
	.
	.
	.
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
example Codes
#define ADC_DEFAULT_JUNK_SAMPLING_TIMES
getAdcValue(SPI_MODE_ADC0, 0, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES)
*/
unsigned int getAdcValue(unsigned char spiDeviceNumber, unsigned char numberOfPort, unsigned int samplingTimes, unsigned char junkTimes)
{
		unsigned int SPI_result = 0;
		unsigned long int avrResult = 0;
		unsigned int samplingTimesTemp = samplingTimes;

		spiDeviceHold(spiDeviceNumber);
		
		for(junkTimes=junkTimes;junkTimes!=0; junkTimes--)
		{
		//ADC dummy Communication, that is two times.
			spiTransCeive(GET_ADC_PORT_INSTRUCTION(numberOfPort));
			spiTransCeive(GET_ADC_PORT_INSTRUCTION(numberOfPort));

			spiTransCeive(GET_ADC_PORT_INSTRUCTION(numberOfPort));
			spiTransCeive(GET_ADC_PORT_INSTRUCTION(numberOfPort));
		}

		for(samplingTimes=samplingTimes; samplingTimes!=0; samplingTimes--)
		{
		//ADC dummy Communication, that is two times.
			spiTransCeive(GET_ADC_PORT_INSTRUCTION(numberOfPort));
			spiTransCeive(GET_ADC_PORT_INSTRUCTION(numberOfPort));

			SPI_result = spiTransCeive(GET_ADC_PORT_INSTRUCTION(numberOfPort));
			SPI_result = (SPI_result<<8);
			SPI_result |= spiTransCeive(GET_ADC_PORT_INSTRUCTION(numberOfPort));

			//processing of ADC data
			avrResult+=SPI_result;
		}
		spiDeviceRelease();

		return ((unsigned int)(avrResult/samplingTimesTemp));
}
