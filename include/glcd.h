#ifndef _GLCD_H_
#define _GLCD_H_

//SET PORT, WHICH IS USED OUTPUT
#define GLCD_CONTROL_BUS					PORTA
#define GLCD_CONTROL_BUS_DDR				DDRA
#define GLCD_DATA_BUS_OUTPUT				PORTC
#define GLCD_DATA_BUS_INPUT					PINC
#define GLCD_DATA_BUS_DDR					DDRC

/*
	GLCD	LG128643-FMDWH6V
	Description
*/
#define GLCD_t_R	0.025
#define GLCD_t_F	0.025

#define GLCD_t_C	1//us
#define GLCD_t_WH	0.45
#define GLCD_t_WL	0.45

#define GLCD_t_ASU	0.14
#define GLCD_t_AH	0.01
#define GLCD_t_DSU	0.2
#define GLCD_t_D	0.32
#define GLCD_t_DWH	0.01
#define GLCD_t_DHR	0.02

/*
	GLCD HardWare Discription
*/
/*Control Port IO PIN*/
/*//processing in hardware(in circuit)
*/
#define GLCD_DATA_BIT_POSITION_START			0
#define GLCD_DATA_BIT_POSITION_END				8

#define GLCD_CONTROL_BIT_POSITION_START			3
#define GLCD_CONTROL_BIT_POSITION_END			8//(add 1 at the lastes bit position)

// X->	//#define GLCD_BIT_POSITION_RST	8//in datasheet, reset signal is L. But in circuit, RST signal is passed throught inverter(Not gate). so designer control that RST signal inverted.
#define GLCD_BIT_POSITION_CS2					7//right display penal. select H, not select L.
#define GLCD_BIT_POSITION_CS1					5//left display penal. select H, not select L.
#define GLCD_BIT_POSITION_E						6//when read data, H. when write data, H -> L 
#define GLCD_BIT_POSITION_RW					3//H read. L write.
#define GLCD_BIT_POSITION_RS					4//display on/off. H display, L Instruction Mode.

//LCD
#define BOTH_SIDE_BLANK					0x00
#define BOTH_SIDE_COLOR					0x00//0x00 WHITE, 0xFF BLACK

#define LCD_NUMBER_OF_CHIP				0x02

#define LCD_PENAL_SECTION_LENGTH		0x40//64
#define LCD_PENAL_HEIGHT				0x40//64

#define LCD_PENAL_LENGTH		(LCD_PENAL_SECTION_LENGTH*LCD_NUMBER_OF_CHIP)//128

#if	LCD_PENAL_LENGTH < 256
#define LCD_COUNTER_DATA_TYPE		unsigned char
#else
#define LCD_COUNTER_DATA_TYPE		unsigned int
#endif

#if	LCD_PENAL_LENGTH < 256
#define LCD_COUNTER_DATA_TYPE_X		unsigned char
#else
#define	LCD_COUNTER_DATA_TYPE_X		unsigned int
#endif

#if	LCD_PENAL_HEIGHT < 256
#define LCD_COUNTER_DATA_TYPE_Y		unsigned char
#else
#define	LCD_COUNTER_DATA_TYPE_Y		unsigned int
#endif

#define LCD_DATA_BUS_TYPE			unsigned char
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char maximumNumberOfCharacterInPage = ((LCD_PENAL_LENGTH - 1) / (FONT_SIZE_Y + FONT_BLANK_FRONT));
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BELOW DEFINES MUST ARE MUST USED WITH CONTROL_MODE. BE USED WITH is mean that CONTROL_MODE and CONTROL_INSTRUCT OR OPERATE EACH OTHER 
#define CONTROL_RST						(1<<GLCD_BIT_POSITION_RST)//0010 0000 -> XX1X XXXX
#define CONTROL_CS2						(1<<GLCD_BIT_POSITION_CS2)//0001 0000 -> XX01 0XXX
#define CONTROL_CS1						(1<<GLCD_BIT_POSITION_CS1)//0000 1000 -> XX00	1XXX
#define CONTROL_CS0						(CONTROL_CS1|CONTROL_CS2)//0001 1000 -> XX01 1XXX
#define CONTROL_ENABLE_BIT_SET			(1<<GLCD_BIT_POSITION_E)//0000 0100 -> XX0X X1XX//Using OR operation
#define CONTROL_ENABLE_BIT_RESET		(~CONTROL_ENABLE_BIT_SET)//1111 1011 //USING AND OPERATION
#define CONTROL_RW_SET					(1<<GLCD_BIT_POSITION_RW)
#define CONTROL_RW_RESET				(~CONTROL_RW_SET)
#define CONTROL_READ					(1<<GLCD_BIT_POSITION_RW)
#define CONTROL_WRITE					(0<<GLCD_BIT_POSITION_RW)
#define CONTROL_RS_BIT_SET				(1<<GLCD_BIT_POSITION_RS)
#define CONTROL_RS_BIT_RESET			(~CONTROL_RS_BIT_SET)
#define CONTROL_SELECT_DISPLAY_REGISTER	(1<<GLCD_BIT_POSITION_RS)
#define CONTROL_SELECT_INST_REGISTER	(0<<GLCD_BIT_POSITION_RS)

#define CONTROL_BITS					(CONTROL_CS0|CONTROL_RW_SET|CONTROL_RS_BIT_SET)
#define CONTROL_ALL_BITS				(CONTROL_BITS|CONTROL_ENABLE_BIT_SET)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CONTROL_INSTRUCTION					ALL_ZERO
#define CONTROL_WRITE_DATA					(CONTROL_INSTRUCTION|CONTROL_RS_BIT_SET)
#define CONTROL_STATUS_READ					(CONTROL_INSTRUCTION|CONTROL_RW_SET)
#define CONTROL_READ_DATA					(CONTROL_INSTRUCTION|CONTROL_RS_BIT_SET|CONTROL_RW_SET)
/////////////////////////////////////////////////////////////////
#define CONTROL_INSTRUCTION_CS0				(CONTROL_INSTRUCTION|CONTROL_CS0)
#define CONTROL_INSTRUCTION_CS1				(CONTROL_INSTRUCTION|CONTROL_CS1)
#define CONTROL_INSTRUCTION_CS2				(CONTROL_INSTRUCTION|CONTROL_CS2)

#define CONTROL_WRITE_DATA_CS0				(CONTROL_WRITE_DATA|CONTROL_CS0)
#define CONTROL_WRITE_DATA_CS1				(CONTROL_WRITE_DATA|CONTROL_CS1)
#define CONTROL_WRITE_DATA_CS2				(CONTROL_WRITE_DATA|CONTROL_CS2)

#define CONTROL_STATUS_READ_CS0 			(CONTROL_STATUS_READ|CONTROL_CS0)
#define CONTROL_STATUS_READ_CS1 			(CONTROL_STATUS_READ|CONTROL_CS1)
#define CONTROL_STATUS_READ_CS2 			(CONTROL_STATUS_READ|CONTROL_CS2)

#define CONTROL_READ_DATA_CS1				(CONTROL_READ_DATA|CONTROL_CS1)
#define CONTROL_READ_DATA_CS2				(CONTROL_READ_DATA|CONTROL_CS2)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define INST_DISPLAY_OFF						0x3E//00 11 1110	
#define INST_DISPLAY_ON							0x3F//00 11 1111
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DEFAULT PLOT AT 64*64 LCD MODULE. CUATION, X is named PAGE, which is have 8 bit. See the datasheet.
#define INST_DEFAULT_PAGE_START					0xB8
#define INST_DEFAULT_COLUMN_START				0x40
#define INST_DEFAULT_LINE_START					0xC0
//DEFAULT PLOT AT 64*64 LCD MODULE. CUATION, X is named PAGE, which is have 8 bit. See the datasheet.
#define INST_DEFAULT_PAGE_END					0xBF
#define INST_DEFAULT_COLUMN_END					0x7F
#define INST_DEFAULT_LINE_END					0xFF
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define STATUS_RESET							0x10//0001 0000
#define STATUS_ONOFF							0x20//0010 0000
#define STATUS_BUSY								0x80//1000 0000
#define STATUS_ALL_BITS							(STATUS_RESET|STATUS_ONOFF|STATUS_BUSY)

#define MODE_READ_DATA							0x03//0000 0011 -> XXXX XX11
//below delay used in write instruction to CS1 and CS2.
#define WRITE_DELAY_TIME_U						3//us
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* using with offset */ 
#define PAGE0									0xB8

#define PAGE(n)									(PAGE0+n-1)

#define PAGE1									0xB8
#define PAGE2									0xB9
#define PAGE3									0xBA
#define PAGE4									0xBB
#define PAGE5									0xBC
#define PAGE6									0xBD
#define PAGE7									0xBE
#define PAGE8									0xBF
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	what is mean that line address?

	line address is start address of the GLCD of top on screen.
	for example, if code is set a line address that 0xc0, a screen is a top of GLCD led cell display 0xc0 that is writted at a glcd ram.
*/
/* if needs, using with PAGE_HEGHT */
#define PAGE0_TO_LINE_ADDR						0xC0

#define PAGE_HEIGHT								0x08
/*
	Page to Line Address
*/
#define PAGE1_TO_LINE_ADDR						0xC0
#define PAGE2_TO_LINE_ADDR						0xC8
#define PAGE3_TO_LINE_ADDR						0xD0
#define PAGE4_TO_LINE_ADDR						0xD8
#define PAGE5_TO_LINE_ADDR						0xE0
#define PAGE6_TO_LINE_ADDR						0xE8
#define PAGE7_TO_LINE_ADDR						0xF0
#define PAGE8_TO_LINE_ADDR						0xF8
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void glcdWriteCommon(unsigned char control, unsigned char commandOrData);
unsigned char glcdReadByteCommon
	(
		unsigned char columnOffset, 
		unsigned char commandOfReadingStatusOrdata
	);
void initGlcd(void);//control, data output bus
void resetGlcd();
void setGlcd();
unsigned char getByteFromGlcd(unsigned char pageOffset, unsigned char columnOffset);
void getGlcdByteArrayDataAtPage(unsigned char page, unsigned char *savedCharArray);
void copyGlcdPageToPage(unsigned char fromPageOffset, unsigned char toPageOffset);
void putDotGlcdAxis(unsigned char axisOfX, unsigned char axisOfY);
void putUpOnDotGlcdAxis(unsigned char axisOfX, unsigned char axisOfY);
void shiftRightOneCellInPage(unsigned char fromAxisOfX, unsigned char toAxisOfX, unsigned char fromPageOffset, unsigned char toPageOffset);
void shiftLeftOneCellInPage(unsigned char fromAxisOfX, unsigned char toAxisOfX, unsigned char fromPageOffset, unsigned char toPageOffset);
void copyByteGlcdAxis(unsigned char fromPageOffset, unsigned char toPageOffset, unsigned char axisOfX);
void putByteDataGlcd(unsigned char pageOffset,unsigned char columnOffset, unsigned char data);
void overwriteByteArrayDataInGlcdToPage
	(
		unsigned char pageOffset, 
		unsigned char *dataArray, 
		unsigned char arrayOrStringOrPictureLength
	);
void overWriteByteArrayDataInGlcdSetStartColumnOffsetToPage
	(
		unsigned char pageOffset, 
		unsigned char startColumnOffset,
		unsigned char *dataArray, 
		unsigned char arrayOrStringOrPictureLength
	);
void removeDotGlcd(unsigned char axisOfX, unsigned char axisOfY);
void putByteInGlcdAtPage(unsigned char page, char *p);
int englishFontFinder(char c);
void putStringInGlcdAtPageAddress(unsigned char page, char *p);
void putStringInGlcdAtPage(unsigned char page, char *p);//programmer can use this function with page address or page offset
unsigned char getUsingdoubleToBarGraphAltitude(double value);//¸·´ë ±×·¡ÇÁÀÇ ±æÀÌ(³ôÀÌ)¸¦ ±¸ÇÔ
void getBarGraphComplementAddressAndPage
	(
		unsigned char barGraphAltitude,
		unsigned char *dotChangeStartPageAddr,
		unsigned char *dotChangeLineAddr,
		unsigned char *numberOfOneBitPosition
	);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	used to touch penal calibration, GLCD structure.
*/
void getBarGraphComplementAddressAndPage
	(
		unsigned char barGraphAltitude,
		unsigned char *dotChangeStartPageAddr,
		unsigned char *dotChangeLineAddr,
		unsigned char *numberOfOneBitPosition
	);

#endif
