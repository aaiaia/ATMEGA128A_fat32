#include "spi.h"

unsigned char SPI_Master_Init()
{
	SPI_DDR &= ~SPI_DEVICE_SELECT_MUX_SET_DDR;//Because SPI port was already used to another purpose like GPIO, so input output setting have to clear.
	SPI_DDR |= SPI_DEVICE_SELECT_MUX_SET_DDR;//To using MUX input signal, set output.

	SPI_DDR |= (SPI_SCK_SET_OUTPUT|SPI_MOSI_SET_OUTPUT|SPI_CS_SET_OUTPUT);//output
	SPI_DDR &= ~SPI_MISO_SET_OUTPUT;//input

	SPCR |= ((1<<SPE) | (1<<MSTR));//spi enable, but not set IO pulse shape.

	SPI_DEVICE_DISABLE;
	return 0;
}

unsigned char spiTransCeive(unsigned char transferDeviceCommand)
{
	SPDR = transferDeviceCommand;
	while(!(SPSR&(1<<SPIF)));

	transferDeviceCommand = SPDR;

	return transferDeviceCommand;
}

unsigned char spiTransfer(unsigned char data)
{
	SPDR = data;
	while(!(SPSR&(1<<SPIF)));

	data = SPDR;

	return data;
}

unsigned char spiReceive()
{
	unsigned char data;
	SPDR = 0xFF;
	while(!(SPSR&(1<<SPIF)));

	data = SPDR;

	return data;
}

#define DEVICE_SELECTOR_T_pd	1//us

void spiDeviceHold(unsigned char spiDeviceNumber)
{
	SPI_DEVICE_HOLD(SPI_DEVICE_SEL(spiDeviceNumber));
	spiReceive();
	SPI_DEVICE_ENABLE;//CD bit was "0"
	_delay_us(DEVICE_SELECTOR_T_pd);
	_delay_us(DEVICE_SELECTOR_T_pd);
	_delay_us(DEVICE_SELECTOR_T_pd);
	_delay_us(DEVICE_SELECTOR_T_pd);
}

void spiDeviceRelease()
{
	SPI_DEVICE_RELEASE;
	SPI_DEVICE_DISABLE;
	spiReceive();
	_delay_us(DEVICE_SELECTOR_T_pd);
	_delay_us(DEVICE_SELECTOR_T_pd);
	_delay_us(DEVICE_SELECTOR_T_pd);
	_delay_us(DEVICE_SELECTOR_T_pd);
}
