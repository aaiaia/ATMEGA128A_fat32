#ifndef _SPI_H
#define _SPI_H
/*
	description of SPI communication mode
*/
#define SPI_MODE_SD_CARD	0x00
#define SPI_MODE_ADC0		0x01
#define SPI_MODE_ADC1		0x02
#define SPI_MODE_TOUCH		0x03

#define GET_ADC_PORT_INSTRUCTION(adcAddr)		(adcAddr<<3)//that is instruction of ADC128S102.
//#define TRANSCEIVE_SPI_DATA(data)	( SPDR=data; while(!(SPCR&(1<<SPIF))); )

#define SPI_DDR				DDRB
#define SPI_PORT			PORTB

#define SPI_CS_SET_OUTPUT	(1<<0)
#define SPI_SCK_SET_OUTPUT	(1<<1)
#define SPI_MOSI_SET_OUTPUT	(1<<2)
#define SPI_MISO_SET_OUTPUT	(1<<3)

//#define SPI_DEVICE_SELECT_MUX_SET_DDR ((1<<4)|(1<<5)|(1<<6)|(1<<7))
#define SPI_DEVICE_SELECT_BIT_START_POSITION	4

#define SPI_DEVICE_SELECT_MUX_SET_DDR ((1<<SPI_DEVICE_SELECT_BIT_START_POSITION)|(1<<(SPI_DEVICE_SELECT_BIT_START_POSITION+1)))

#define SPI_DEVICE_SEL(spi_device_number)	(SPI_DEVICE_SELECT_MUX_SET_DDR&((spi_device_number)<<(SPI_DEVICE_SELECT_BIT_START_POSITION)))
#define SPI_DEVICE_HOLD(spi_device_addr)	(SPI_PORT|=(spi_device_addr))
#define SPI_DEVICE_RELEASE				(SPI_PORT&=(~SPI_DEVICE_SELECT_MUX_SET_DDR))

#define SPI_DEVICE_ENABLE	(SPI_PORT &= (~SPI_CS_SET_OUTPUT))//CD bit was "0"
#define	SPI_DEVICE_DISABLE	(SPI_PORT |= SPI_CS_SET_OUTPUT)//CS bit was "1"

////////////////////////////// Definition of Each SPI device MODE /////////////////////////
#define SPI_SD_CARD_SPCR_SET				(SPCR = ((1<<SPE)|(1<<MSTR)))
#define SPI_ADC_SPCR_SET					(SPCR = ((1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)))


#define SPI_SD_CARD_SPSR_SET_HIGH_SPEED		(SPSR = (1<<SPI2X))
#define SPI_ADC_SPSR_SET					(SPSR = (1<<SPI2X))

#endif
