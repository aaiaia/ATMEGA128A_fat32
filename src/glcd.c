#include "glcd.h"
/* basic function of GLCD, read and write */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	default write function
*/
void glcdWriteCommon(unsigned char control, unsigned char commandOrData)
{	
/*
	waiting process START
*/
//	waiting process START using switch
	switch(control&CONTROL_CS0)
	{
		case CONTROL_CS0:
			GLCD_CONTROL_BUS &= ~CONTROL_ALL_BITS;
			GLCD_CONTROL_BUS |= control;
			_delay_us(0.2);
			GLCD_CONTROL_BUS |= CONTROL_ENABLE_BIT_SET;
			GLCD_DATA_BUS_OUTPUT = commandOrData;

			_delay_us(WRITE_DELAY_TIME_U);
			GLCD_CONTROL_BUS &= CONTROL_ENABLE_BIT_RESET;
			_delay_us(WRITE_DELAY_TIME_U);
			GLCD_CONTROL_BUS &= ~CONTROL_ALL_BITS;
			_delay_us(WRITE_DELAY_TIME_U);
		default :
			return;

		case CONTROL_CS1:
		case CONTROL_CS2:
			GLCD_DATA_BUS_OUTPUT = ALL_ZERO;
			GLCD_DATA_BUS_DDR = INPUT;

						//GLCD_CONTROL_BUS &= CONTROL_ALL_BITS;//reset GLCD control bits
			_delay_us(GLCD_t_F+(GLCD_t_WL-GLCD_t_ASU));

			GLCD_CONTROL_BUS |= CONTROL_READ;//now on read status.

			GLCD_CONTROL_BUS |= (control&CONTROL_CS0);

			_delay_us(GLCD_t_R+GLCD_t_F+GLCD_t_ASU);

			GLCD_CONTROL_BUS |= CONTROL_ENABLE_BIT_SET;//make positive edge.
			//_delay_us(GLCD_t_D+GLCD_t_F);
			_delay_us(GLCD_t_R+GLCD_t_DSU);

		//	data = GLCD_DATA_BUS_INPUT;
			while((GLCD_DATA_BUS_INPUT&STATUS_ALL_BITS) != ALL_ZERO){};
			GLCD_CONTROL_BUS &= CONTROL_ENABLE_BIT_RESET;
			_delay_us(GLCD_t_F+GLCD_t_AH);

						//GLCD_CONTROL_BUS &= CONTROL_ALL_BITS;//reset GLCD control bits
			GLCD_DATA_BUS_OUTPUT = ALL_ZERO;
			_delay_us(GLCD_t_F+GLCD_t_DSU-GLCD_t_AH);

			GLCD_DATA_BUS_DDR = OUTPUT;
	}
/*
	waiting process END
*/
	GLCD_CONTROL_BUS &= ~CONTROL_ALL_BITS;
	_delay_us(GLCD_t_F+GLCD_t_WL-GLCD_t_ASU);

	GLCD_CONTROL_BUS |= control;
	_delay_us(GLCD_t_R+GLCD_t_F+GLCD_t_ASU);

	GLCD_CONTROL_BUS |= CONTROL_ENABLE_BIT_SET;
	_delay_us(GLCD_t_R+GLCD_t_D);

	GLCD_DATA_BUS_OUTPUT = commandOrData;
	_delay_us(GLCD_t_R+GLCD_t_F+GLCD_t_WH-GLCD_t_D);

	GLCD_CONTROL_BUS &= CONTROL_ENABLE_BIT_RESET;
	_delay_us(GLCD_t_F+GLCD_t_AH);

	GLCD_CONTROL_BUS &= ~CONTROL_ALL_BITS;
	_delay_us(GLCD_t_F+GLCD_t_R+GLCD_t_DHR-GLCD_t_AH-GLCD_t_F);

	GLCD_DATA_BUS_OUTPUT = ALL_ZERO;
	_delay_us(GLCD_t_WL-GLCD_t_DHR-GLCD_t_F-GLCD_t_R-GLCD_t_R);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Default Reading Status Bits
*/
unsigned char glcdReadByteCommon
	(
		unsigned char columnOffset, 
		unsigned char commandOfReadingStatusOrdata
	)
{
	unsigned char data = 0;

	/* statuc check START */
	GLCD_DATA_BUS_OUTPUT = ALL_ZERO;
	GLCD_DATA_BUS_DDR = INPUT;
	GLCD_CONTROL_BUS &= ~CONTROL_ALL_BITS;
	_delay_us((GLCD_t_WL-GLCD_t_ASU)+GLCD_t_R*2);

	GLCD_CONTROL_BUS |= CONTROL_READ;//now on read status.
	//GLCD_CONTROL_BUS |= commandOfReadStatusOrdata;

	if(columnOffset < LCD_PENAL_SECTION_LENGTH)
	{
		GLCD_CONTROL_BUS |= CONTROL_CS1;
	}
	else
	{
		GLCD_CONTROL_BUS |= CONTROL_CS2;
	}
	_delay_us(GLCD_t_R+GLCD_t_ASU);

	GLCD_CONTROL_BUS |= CONTROL_ENABLE_BIT_SET;//make positive edge.
	_delay_us(GLCD_t_R+GLCD_t_D);

	while((GLCD_DATA_BUS_INPUT&STATUS_ALL_BITS) != ALL_ZERO){};
	_delay_us(GLCD_t_WH-GLCD_t_D);

	GLCD_CONTROL_BUS &= CONTROL_ENABLE_BIT_RESET;
	_delay_us(GLCD_t_F+GLCD_t_AH);

			GLCD_CONTROL_BUS |= commandOfReadingStatusOrdata;
	_delay_us(GLCD_t_R+GLCD_t_F+GLCD_t_DHR);
	/* statuc check END */


	GLCD_CONTROL_BUS |= CONTROL_ENABLE_BIT_SET;//make positive edge.
	_delay_us(GLCD_t_R+GLCD_t_D);

	data = GLCD_DATA_BUS_INPUT;
	_delay_us(GLCD_t_WH-GLCD_t_D);

	GLCD_CONTROL_BUS &= CONTROL_ENABLE_BIT_RESET;
	_delay_us(GLCD_t_F+GLCD_t_AH);

	GLCD_CONTROL_BUS &= ~CONTROL_ALL_BITS;
	_delay_us(GLCD_t_R+GLCD_t_F+GLCD_t_DHR);

	GLCD_DATA_BUS_DDR = OUTPUT;
	_delay_us(GLCD_t_F+GLCD_t_WL-GLCD_t_DHR+GLCD_t_R);
	return data;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initGlcd(void)//control, data output bus
{
	unsigned char i=0;

	for(i=GLCD_CONTROL_BIT_POSITION_START; i<GLCD_CONTROL_BIT_POSITION_END; i++)//control IO port is decided along that GLCD_CONTROL_BIT_POSITION_START and GLCD_CONTROL_BIT_POSITION_END.
	{
		SET_OUTPUT(i, GLCD_CONTROL_BUS_DDR);
	}

	for(i=GLCD_DATA_BIT_POSITION_START; i<GLCD_DATA_BIT_POSITION_END; i++)//data bus configuration
	{
		SET_OUTPUT(i, GLCD_DATA_BUS_DDR);
	}

	GLCD_CONTROL_BUS &= ~CONTROL_ALL_BITS;
	GLCD_DATA_BUS_OUTPUT = ALL_ZERO;

	glcdWriteCommon(CONTROL_CS0, INST_DISPLAY_ON);
	glcdWriteCommon(CONTROL_CS0, INST_DEFAULT_COLUMN_START);
	glcdWriteCommon(CONTROL_CS0, INST_DEFAULT_LINE_START);//indicated line address of ram in GLCD, is displayed at a top of screen.
	setGlcd();
	resetGlcd();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void resetGlcd()
{
	LCD_COUNTER_DATA_TYPE i, j;
	glcdWriteCommon(CONTROL_CS0, INST_DEFAULT_COLUMN_START);

	for(i = INST_DEFAULT_PAGE_START; i <= INST_DEFAULT_PAGE_END; i++)//X page
	{
		glcdWriteCommon(CONTROL_CS0, i);
		for(j = INST_DEFAULT_COLUMN_START; j <= INST_DEFAULT_COLUMN_END; j++)
		{
			glcdWriteCommon(CONTROL_WRITE_DATA_CS0, ALL_ZERO);//write data
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setGlcd()
{
	LCD_COUNTER_DATA_TYPE i, j;
	glcdWriteCommon(CONTROL_CS0, INST_DEFAULT_COLUMN_START);

	for(i = INST_DEFAULT_PAGE_START; i <= INST_DEFAULT_PAGE_END; i++)//X page
	{
		glcdWriteCommon(CONTROL_CS0, i);

		for(j = INST_DEFAULT_COLUMN_START; j <= INST_DEFAULT_COLUMN_END; j++)
		{
			glcdWriteCommon(CONTROL_WRITE_DATA_CS0, ALL_ONE);//write data
		}

	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char getByteFromGlcd(unsigned char pageOffset, unsigned char columnOffset)
{
	glcdWriteCommon(CONTROL_CS0, (INST_DEFAULT_PAGE_START+pageOffset));

	if(columnOffset < LCD_PENAL_SECTION_LENGTH)
	{
		glcdWriteCommon(CONTROL_CS1, (INST_DEFAULT_COLUMN_START+columnOffset));//I have no idea, that why subtract 1 from default colum start address + offsets
	}
	else
	{
		glcdWriteCommon(CONTROL_CS2, (INST_DEFAULT_COLUMN_START-LCD_PENAL_SECTION_LENGTH+columnOffset));//I have no idea, that why subtract 1 from default colum start address + offsets
	}

	glcdReadByteCommon(columnOffset, CONTROL_SELECT_DISPLAY_REGISTER);//dummy

	return glcdReadByteCommon(columnOffset, CONTROL_SELECT_DISPLAY_REGISTER);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void getGlcdByteArrayDataAtPage(unsigned char pageOffset, unsigned char *savedCharArray)
{
	unsigned char columnOffset=0;

	glcdWriteCommon(CONTROL_CS0, (INST_DEFAULT_PAGE_START+pageOffset));
	glcdWriteCommon(CONTROL_CS0, INST_DEFAULT_COLUMN_START);
	
	//glcdWriteCommon(CONTROL_CS1, INST_DEFAULT_COLUMN_START);

	glcdReadByteCommon(columnOffset, CONTROL_READ_DATA_CS1);//dummy

	for(columnOffset=0; columnOffset<LCD_PENAL_SECTION_LENGTH; columnOffset++)
	{
		savedCharArray[columnOffset] = glcdReadByteCommon(columnOffset, CONTROL_READ_DATA_CS1);
	}


	//glcdWriteCommon(CONTROL_CS2, INST_DEFAULT_COLUMN_START);

	glcdReadByteCommon(columnOffset, CONTROL_READ_DATA_CS2);//dummy

	for(columnOffset=columnOffset; columnOffset<LCD_PENAL_SECTION_LENGTH*2; columnOffset++)
	{
		savedCharArray[columnOffset] = glcdReadByteCommon(columnOffset, CONTROL_READ_DATA_CS2);
	}
	
	savedCharArray[columnOffset++] = '\0';
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	void copyGlcdPageToPage(unsigned char fromPageOffset, unsigned char toPageOffset)
	have a issue that, when MCU process this function
*/
void copyGlcdPageToPage(unsigned char fromPageOffset, unsigned char toPageOffset)
{
	unsigned char temp[LCD_PENAL_LENGTH+1] = {0};

	getGlcdByteArrayDataAtPage(fromPageOffset, temp);

	overwriteByteArrayDataInGlcdToPage(toPageOffset, temp, LCD_PENAL_LENGTH);
}
void reversePage(unsigned char pageOffset)
{
	unsigned char temp[LCD_PENAL_LENGTH+1] = {0};
	unsigned char i;
	
	getGlcdByteArrayDataAtPage(pageOffset, temp);

	for(i=0; i<LCD_PENAL_LENGTH; i++)
	{
		(*(temp+i))=~(*(temp+i));
	}

	overwriteByteArrayDataInGlcdToPage(pageOffset, temp, LCD_PENAL_LENGTH);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void putDotGlcdAxis(unsigned char axisOfX, unsigned char axisOfY)
{
	putByteDataGlcd
	(
		(axisOfY/PAGE_HEIGHT),
		axisOfX,
		(1<<(axisOfY%PAGE_HEIGHT))
	);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void putUpOnDotGlcdAxis(unsigned char axisOfX, unsigned char axisOfY)
{
	putByteDataGlcd
	(
		(axisOfY/PAGE_HEIGHT),
		axisOfX,
		(
			(1<<(axisOfY%PAGE_HEIGHT))
			|
			getByteFromGlcd((axisOfY/PAGE_HEIGHT), axisOfX)	
		)
	);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void shiftRightOneCellInPage(unsigned char fromAxisOfX, unsigned char toAxisOfX, unsigned char fromPageOffset, unsigned char toPageOffset)
{//axisOfX and axisOfY is indiacte to a area of GLCD consist of pixel.
	unsigned char preData=0;
	unsigned char columnCounterOffset=toAxisOfX, pageCounterOffset=fromPageOffset;

	for(pageCounterOffset=fromPageOffset; pageCounterOffset<(toPageOffset+1); pageCounterOffset++)//page count
	{
		for(columnCounterOffset=toAxisOfX; columnCounterOffset!=fromAxisOfX; columnCounterOffset--)
		{
			preData = getByteFromGlcd(pageCounterOffset, (columnCounterOffset-1));
//			_delay_us(2.7);
			putByteDataGlcd(pageCounterOffset, columnCounterOffset, preData);
		}
		putByteDataGlcd(pageCounterOffset, columnCounterOffset, ALL_ZERO);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void shiftLeftOneCellInPage(unsigned char fromAxisOfX, unsigned char toAxisOfX, unsigned char fromPageOffset, unsigned char toPageOffset)
{//axisOfX and axisOfY is indiacte to a area of GLCD consist of pixel.
	unsigned char nextData=0;
	unsigned char columnCounterOffset=toAxisOfX, pageCounterOffset=fromPageOffset;

	for(pageCounterOffset=fromPageOffset; pageCounterOffset<(toPageOffset+1); pageCounterOffset++)//page count
	{
		for(columnCounterOffset=fromAxisOfX; columnCounterOffset!=toAxisOfX; columnCounterOffset++)
		{
			nextData = getByteFromGlcd(pageCounterOffset, (columnCounterOffset+1));
//			_delay_us(2.7);
			putByteDataGlcd(pageCounterOffset, columnCounterOffset, nextData);
		}
		putByteDataGlcd(pageCounterOffset, columnCounterOffset, ALL_ZERO);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void copyByteGlcdAxis(unsigned char fromPageOffset, unsigned char toPageOffset, unsigned char axisOfX)
{
	unsigned char temp = getByteFromGlcd(fromPageOffset, axisOfX);
	putByteDataGlcd(toPageOffset, axisOfX,  temp);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void putByteDataGlcd(unsigned char pageOffset,unsigned char columnOffset, unsigned char data)
{
	glcdWriteCommon(CONTROL_CS0, INST_DEFAULT_PAGE_START + pageOffset);

	if(columnOffset < LCD_PENAL_SECTION_LENGTH)
	{
		glcdWriteCommon(CONTROL_CS1, INST_DEFAULT_COLUMN_START + columnOffset);
		glcdWriteCommon(CONTROL_WRITE_DATA_CS1, data);
	}
	else
	{
		glcdWriteCommon(CONTROL_CS2, INST_DEFAULT_COLUMN_START - LCD_PENAL_SECTION_LENGTH + columnOffset);
		glcdWriteCommon(CONTROL_WRITE_DATA_CS2, data);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	overwriteByteArrayDataInGlcdToPage is derived from overwriteByteArrayDataInGlcdToPage
*/
void overwriteByteArrayDataInGlcdToPage
	(
		unsigned char pageOffset, 
		unsigned char *dataArray, 
		unsigned char arrayOrStringOrPictureLength
	)//dataArray is same that char Array.
{
	unsigned char columnOffset=0;//this variable is same dataArrayIndex
	glcdWriteCommon(CONTROL_CS0, INST_DEFAULT_PAGE_START + pageOffset);

	glcdWriteCommon(CONTROL_CS1, INST_DEFAULT_COLUMN_START);	
	while((columnOffset<LCD_PENAL_SECTION_LENGTH)&&(columnOffset<arrayOrStringOrPictureLength))
//	while((columnOffset<LCD_PENAL_SECTION_LENGTH)&&(dataArray[columnOffset]!='\0'))
//	for(columnOffset=0; ((columnOffset<LCD_PENAL_SECTION_LENGTH)&&(dataArray[columnOffset]!='\0')); columnOffset++)//
//	for(columnOffset=0; ((columnOffset<LCD_PENAL_SECTION_LENGTH)&&(columnOffset<=(sizeof(dataArray)/sizeof(unsigned char)))); columnOffset++)
	{
		glcdWriteCommon(CONTROL_WRITE_DATA_CS1, dataArray[columnOffset++]);

	}
	/*After Maintain*/

	glcdWriteCommon(CONTROL_CS2, INST_DEFAULT_COLUMN_START);
	while((columnOffset<LCD_PENAL_SECTION_LENGTH*2)&&(columnOffset<arrayOrStringOrPictureLength))
//	while((columnOffset<LCD_PENAL_SECTION_LENGTH*2)&&(dataArray[columnOffset]!='\0'))
//	for(columnOffset=columnOffset; ((columnOffset<LCD_PENAL_SECTION_LENGTH*2)&&(dataArray[columnOffset]!='\0')); columnOffset++)
//	for(columnOffset=columnOffset; ((columnOffset<LCD_PENAL_SECTION_LENGTH*2)&&(columnOffset<=(sizeof(dataArray)/sizeof(unsigned char)))); columnOffset++)
	{
		glcdWriteCommon(CONTROL_WRITE_DATA_CS2, dataArray[columnOffset++]);		
	}
	/*After Maintain*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void overWriteByteArrayDataInGlcdSetStartColumnOffsetToPage
	(
		unsigned char pageOffset, 
		unsigned char startColumnOffset,
		unsigned char *dataArray, 
		unsigned char arrayOrStringOrPictureLength
	)//dataArray is same that char Array.
{
	unsigned int dataArrayIndex = 0;
	glcdWriteCommon(CONTROL_CS0, INST_DEFAULT_PAGE_START + pageOffset);

	glcdWriteCommon(CONTROL_CS1, INST_DEFAULT_COLUMN_START + startColumnOffset);
	while((startColumnOffset<LCD_PENAL_SECTION_LENGTH)&&(dataArrayIndex<arrayOrStringOrPictureLength))
//		while((startColumnOffset<LCD_PENAL_SECTION_LENGTH)&&(startColumnOffset<=(sizeof(dataArray)/sizeof(unsigned char))))
	{
		glcdWriteCommon(CONTROL_WRITE_DATA_CS1, dataArray[dataArrayIndex++]);
		startColumnOffset++;
//			dataArrayIndex++;
	}

	glcdWriteCommon(CONTROL_CS2, INST_DEFAULT_COLUMN_START + (startColumnOffset - LCD_PENAL_SECTION_LENGTH));
	while((startColumnOffset<(LCD_PENAL_SECTION_LENGTH*2))&&(dataArrayIndex<arrayOrStringOrPictureLength))
//		while((startColumnOffset<LCD_PENAL_SECTION_LENGTH)&&(startColumnOffset<=(sizeof(dataArray)/sizeof(unsigned char))))
	{
		glcdWriteCommon(CONTROL_WRITE_DATA_CS2, dataArray[dataArrayIndex++]);
		startColumnOffset++;
//			dataArrayIndex++;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void removeDotGlcd(unsigned char axisOfX, unsigned char axisOfY)
{
	unsigned char temp = getByteFromGlcd((axisOfY/PAGE_HEIGHT), axisOfX);

	putByteDataGlcd
	(
		(axisOfY/PAGE_HEIGHT),
		axisOfX,
		((~(1<<(axisOfY%PAGE_HEIGHT)))&temp)
	);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Below two Instruction is used whan put specific coordinates*/
//	glcdWriteCommon(CONTROL_ENABLE_CHIP_BOTH_SELECT_MODE_INSTRUCTION, INST_DEFAULT_COLUMN_START);
//	glcdWriteCommon(CONTROL_ENABLE_CHIP_BOTH_SELECT_MODE_INSTRUCTION, INST_DEFAULT_LINE_START);
void putByteInGlcdAtPage(unsigned char page, char *p)
{
	LCD_COUNTER_DATA_TYPE columnOffset = 0;
	LCD_COUNTER_DATA_TYPE i = 0;

	glcdWriteCommon(CONTROL_CS0, page);

/*Below two Instruction is used whan put specific coordinates*/
	glcdWriteCommon(CONTROL_CS0, INST_DEFAULT_COLUMN_START);


	for(columnOffset = 0; columnOffset < LCD_PENAL_LENGTH; columnOffset++)
	{
		if(p[i] == '\0')	break;

		if(columnOffset < LCD_PENAL_SECTION_LENGTH)
		{
			glcdWriteCommon(CONTROL_WRITE_DATA_CS1, p[i]);
		}
		else
		{
			glcdWriteCommon(CONTROL_WRITE_DATA_CS2, p[i]);
		}
	}
	for(columnOffset = 0; columnOffset < LCD_PENAL_LENGTH; columnOffset++)
	{
		if(columnOffset < LCD_PENAL_SECTION_LENGTH)
		{
			glcdWriteCommon(CONTROL_WRITE_DATA_CS1, ALL_ZERO);
		}
		else
		{
			glcdWriteCommon(CONTROL_WRITE_DATA_CS2, ALL_ZERO);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int englishFontFinder(char c)
{
	return (int)(c - ' ');
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(unsigned char page, unsigned char setColumnOffset, char *p)
{
	LCD_COUNTER_DATA_TYPE columnOffset;
	LCD_COUNTER_DATA_TYPE i = 0, j = 0;
	glcdWriteCommon(CONTROL_CS0, page);

//	glcdWriteCommon(CONTROL_CS0, INST_DEFAULT_COLUMN_START);
	if(setColumnOffset < (LCD_PENAL_LENGTH / 2) )
	{
		glcdWriteCommon(CONTROL_CS1, INST_DEFAULT_COLUMN_START + setColumnOffset);
		glcdWriteCommon(CONTROL_CS2, INST_DEFAULT_COLUMN_START);
	}
	else
	{
		glcdWriteCommon(CONTROL_CS2, INST_DEFAULT_COLUMN_START + setColumnOffset - (LCD_PENAL_LENGTH / 2));		
	}

	glcdWriteCommon(CONTROL_CS0, INST_DEFAULT_LINE_START);
	//LCD MOLDUEL 64*64 12 character

	for(columnOffset= setColumnOffset; columnOffset < LCD_PENAL_LENGTH; columnOffset++)
	{
		if((columnOffset%(FONT_5X8_OFFSET)) == ALL_ZERO)
		{
			if(columnOffset < LCD_PENAL_SECTION_LENGTH)
			{
				glcdWriteCommon(CONTROL_WRITE_DATA_CS1, ALL_ZERO);
			}
			else
			{
				glcdWriteCommon(CONTROL_WRITE_DATA_CS2, ALL_ZERO);
			}
			continue;
		}
		else if((columnOffset%FONT_5X8_OFFSET) == FONT_BLANK_FRONT)
		{
			if(*p != '\0')
			{
				i = englishFontFinder(*p);
				j = 0;
				p++;
			}
			else
			{
				i = 0;
				j = 0;
			}
		}
		if(columnOffset < LCD_PENAL_SECTION_LENGTH)
		{
			glcdWriteCommon(CONTROL_WRITE_DATA_CS1, ENGLISH_FONT_5X8[i][j++]);
		}
		else
		{
			glcdWriteCommon(CONTROL_WRITE_DATA_CS2, ENGLISH_FONT_5X8[i][j++]);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void putStringInGlcdAtPageAddress(unsigned char page, char *p)
//{
//	overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(page, 0, p);
//}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void putStringInGlcdAtPage(unsigned char page, char *p)//programmer can use this function with page address or page offset
{
	if((0<=page)&&(page<=7))//using offset
	{
		//putStringInGlcdAtPageAddress(PAGE0+page, p);
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE0+page, 0, p);
	}
	else if((PAGE1<=page)&&(page<=PAGE8))//using pre-defined values
	{
		//putStringInGlcdAtPageAddress(page, p);
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(page, 0, p);
	}
	else
	{
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(page, 0, "error page offset");
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void putStringInGlcdAtPageUsingOffset(unsigned char page, char *p, unsigned char collumnOffset)//programmer can use this function with page address or page offset
{
	if((0<=page)&&(page<=7))//using offset
	{
		//putStringInGlcdAtPageAddress(PAGE0+page, p);
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE0+page, collumnOffset, p);
	}
	else if((PAGE1<=page)&&(page<=PAGE8))//using pre-defined values
	{
		//putStringInGlcdAtPageAddress(page, p);
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(page, collumnOffset, p);
	}
	else
	{
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(page, collumnOffset, "error page offset");
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ONE_CELL_IS	1.5f

unsigned char getUsingdoubleToBarGraphAltitude(double value)//¸·´ë ±×·¡ÇÁÀÇ ±æÀÌ(³ôÀÌ)¸¦ ±¸ÇÔ
{
   unsigned char barGraphAltitude = (unsigned char)((double)(value)/ONE_CELL_IS);//ÇÑ Ä­´ç ³ªÅ¸³¾ ¼ö ÀÖ´Â ¼ýÀÚÀÇ ¹üÀ§·Î ³ª´²¼­ ÃÑ °¹¼ö¸¦ ±¸ÇÔ
 
 
   if( ((double)value-barGraphAltitude) > (ONE_CELL_IS/2.0) )//ÇÑ Ä­ÀÌ ³ªÅ¸³¾ ¼ö ÀÖ´Â ¹üÀ§ÀÇ Àý¹Ýº¸´Ù ³ôÀ¸¸é +1Ä­
   {
      barGraphAltitude++;
   }
 
   return barGraphAltitude;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void getBarGraphComplementAddressAndPage
	(
		unsigned char barGraphAltitude,
		unsigned char *dotChangeStartPageAddr,
		unsigned char *dotChangeLineAddr,
		unsigned char *numberOfOneBitPosition
	)//
{
	//tempBlankSectionÀº 0xC0ºÎÅÍ ½ÃÀÛÇÏ´Â °ªÀÌ 0ÀÎ ÁöÁ¡ÀÇ ±æÀÌ¸¦ ¾ê±âÇÔ
	unsigned char tempBlankSection = ((PAGE4_TO_LINE_ADDR + 0x07) - barGraphAltitude);

	//0ÀÎ ±æÀÌÀÇ /8(ÇÑ ÆäÀÌÁöÀÇ bit °³ ¼ö)À» ÇÏ¸é, 0->1·Î ¹Ù²î´Â pageÀÇ -1 page ÁÖ¼Ò(Page address)°¡ ³ª¿È. µû¶ó¼­ +1 ½ÃÄÑÁÜ
   *dotChangeStartPageAddr  = PAGE1 + ((tempBlankSection / 0x08) + 0x01);
   
   //0ÀÎ ºÎºÐÀÇ ±æÀÌ¸¦ % 8ÇÏ¸é, Èò ºÎºÐÀÇ bitÀ§Ä¡°¡ ³ªÅ¸³². ¿¹¿Ü´Â pageÀÇ °æ°è¿¡ °É¸± °æ¿ì. ÀÌ´Â %8 ÀÌ 0ÀÌ µÇ¾úÀ½À» ³ªÅ¸³½´Ù.
   //¶ÇÇÑ À§ÀÇ page addrÀ» ±¸ÇÏ´Â ºÎºÐ¿¡¼­´Â µü ³ª´©¾î ¶³¾îÁö´Â °æ¿ì(pageÀÇ °æ°è¿¡¼­ °É¸° °æ¿ì), Á¤È®È÷ ³ª´©¾î ¶³¾îÁö±â ¶§¹®¿¡ ¿¹¿ÜÃ³¸®¸¦ ÇÏÁö ¾Ê¾Æµµ µÊ
	//¸¶Ä£°¡Áö·Î, 0ÀÎ ºÎºÐÀÌ ÇØ´ç ÆäÀÌÁö¿¡¼­ ¹Ýµå½Ã Á¸ÀçÇØ¾ßÇÏ¹Ç·Î(¿À·ùÀÇ °¡´É¼ºÀÌ ÀÖÀ»Áöµµ)
	*numberOfOneBitPosition = (tempBlankSection % 0x08);
	if(*numberOfOneBitPosition == 0x00)//pageÀÇ °æ°è¿¡ °É¸° °æ¿ì(0ÀÌ 8bitÁß ¾î´À°÷¿¡µµ Á¸ÀçÇÏÁö ¾Ê°Å³ª, 8bit ¸ðµÎ 0ÀÎ °æ¿ì, ÀüÀÚ´Â ¾Õ¼­ ¾ð±ÞÇÑ °Í°ú °°ÀÌ °í·ÁÇÒ ÇÊ¿ä°¡ ¾ø´Ù)
   {
		*dotChangeLineAddr = (PAGE1_TO_LINE_ADDR + ((*dotChangeStartPageAddr - PAGE1) * 0x08));
   }
   else
   {//(tempBlankSection % 0x08)ÀÇ °á°ú´Â 0ÀÌ Á¸ÀçÇÏ´Â bitÀÇ °³¼öÀÌ´Ù. 1°³ÀÏ °æ¿ì 0¹øÂ° ºñÆ®¸¸, 2°³ÀÏ °æ¿ì 1¹øÂ° ºñÆ®±îÁö¸¸, 7°³ÀÏ °æ¿ì 6¹øÂ° ºñÆ® ±îÁö¸¸, 8°³ÀÏ °æ¿ì 7¹ø? ºñÆ®±îÁö ÀÌ¹Ç·Î,
	//°á±¹ 1ÀÌ À§Ä¡ÇØ¾ßÇÒ °÷ÀÇ À§Ä¡°¡ µÈ´Ù. ((*dotChangeStartPageAddr - PAGE1) * 0x08)´Â PAGE1_TO_LINE_ADDR¿¡ ´ëÀÀÇÏ´Â offset(»ó´ëÀû À§Ä¡)ÀÓ
		*dotChangeLineAddr = (tempBlankSection % 0x08) + PAGE1_TO_LINE_ADDR + ((*dotChangeStartPageAddr - PAGE1) * 0x08);
   }
}
