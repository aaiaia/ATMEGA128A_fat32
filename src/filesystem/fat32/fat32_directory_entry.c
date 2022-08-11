char dirOtherInfoAbstractFromDirectoryEntry(char *str, dirOtherInfo *otherInfo)
{
	(*otherInfo).indicateFirstClustor = abstractLittleEndianTo16Bits(str+LSB_FIRST_CLUSTOR_OFFSET);
	(*otherInfo).indicateFirstClustor |= (((unsigned long)abstractLittleEndianTo16Bits(str+MSB_FIRST_CLUSTOR_OFFSET))<<16);

	(*otherInfo).fileSize=abstractLittleEndianTo32Bits(str+DIR_FILESIZE);
	(*otherInfo).attribute = (*(str+DIR_ATTR_OFFSET));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char dirSimpleNameAbstractFromDirectoryEntry(char *str, directoryName *dirName)
{
	strncpy((*dirName).simple, str, DIR_SIMPLE_NAME_MAXIMUM_LENGTH);//general name copy
	checkAndConvertSimpleNameStringForm((*dirName).simple, DIR_SIMPLE_NAME_MAXIMUM_LENGTH);

	strncpy((*dirName).extension, str+DIR_SIMPLE_NAME_MAXIMUM_LENGTH , DIR_EXTENSION_MAXUMUM_LENGTH);//general name copy
	checkAndConvertSimpleNameStringForm((*dirName).extension, DIR_EXTENSION_MAXUMUM_LENGTH);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char abstractLongNameAndOneEntry(unsigned char i, unsigned char limit, unsigned char cmpOffsetCalibe, char* longName, char *dirEntry)
{
	while(i<limit)
	{
		if(*(dirEntry+i) != 0xff)
		{
			*(longName+((i-cmpOffsetCalibe)/LONG_DIR_NAME_ONE_WORD_SIZE)) = *(dirEntry+i);
		}
		else
		{
			*(longName+((i-cmpOffsetCalibe)/LONG_DIR_NAME_ONE_WORD_SIZE)) = 0;
		}
		i+=LONG_DIR_NAME_ONE_WORD_SIZE;
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char abstractDirLongNameFromDirectoryEntry(char *dirEntry, char *longName)//if long name entry order is exceed maximum longname entry, return exceed entry number.
{
	if( ((*dirEntry)&LONG_NAME_LASTEST_MASK) == LONG_NAME_LASTEST_VALID_VALUE )
	{
		if( !((((*(dirEntry))&LONG_NAME_NUMBER_MASK)-1) <  LONG_NAME_ENTRY_MAXIMUM_NUMBER) )
		{
			*(longName+LONG_NAME_MAXIMUM_LENGTH) = ((*(dirEntry))&LONG_NAME_NUMBER_MASK)-1-LONG_NAME_ENTRY_MAXIMUM_NUMBER;//set long name entry offset...
			return -1;
		}
		*(longName+LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY)=0;
	}

	char *comaparePointer = longName+((((*dirEntry)&LONG_NAME_NUMBER_MASK)-1)*LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY);

	unsigned char cmpOffsetCalibe;

	cmpOffsetCalibe = LONG_DIR_NAME1_OFFSET;
	abstractLongNameAndOneEntry(LONG_DIR_NAME1_OFFSET, LONG_DIR_NAME1_OFFSET+LONG_DIR_NAME1_SIZE, cmpOffsetCalibe, comaparePointer, dirEntry);

	cmpOffsetCalibe += LONG_DIR_NAME2_OFFSET-(LONG_DIR_NAME1_OFFSET+LONG_DIR_NAME1_SIZE);
	abstractLongNameAndOneEntry(LONG_DIR_NAME2_OFFSET, LONG_DIR_NAME2_OFFSET+LONG_DIR_NAME2_SIZE, cmpOffsetCalibe, comaparePointer, dirEntry);

	cmpOffsetCalibe += LONG_DIR_NAME3_OFFSET-(LONG_DIR_NAME2_OFFSET+LONG_DIR_NAME2_SIZE);
	abstractLongNameAndOneEntry(LONG_DIR_NAME3_OFFSET, LONG_DIR_NAME3_OFFSET+LONG_DIR_NAME3_SIZE, cmpOffsetCalibe, comaparePointer, dirEntry);

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void dirDateInfoParseFromDirectoryEntry(char *entry, unsigned char offset, dirDateInfo *date)
{
	if((offset == DIR_CREATION_DATE_OFFSET)||(offset == DIR_LAST_ACCESS_DATE_OFFSET)||(offset == DIR_WRITE_DATE_OFFSET))
	{
		unsigned int temp = abstractLittleEndianTo16Bits(entry+offset);
		(*date).year=((temp&DIR_DATE_YEAR_BIT_MASK)>>DIR_DATE_YEAR_BIT_OFFSET);
		(*date).month=((temp&DIR_DATE_MONTH_BIT_MASK)>>DIR_DATE_MONTH_BIT_OFFSET);
		(*date).day=((temp&DIR_DATE_DAY_BIT_MASK)>>DIR_DATE_DAY_BIT_OFFSET);
	}
	else
	{
		(*date).year=2000;
		(*date).month=1;
		(*date).day=1;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void dirTimeInfoParseFromDirectoryEntry(char *entry, unsigned char offset, dirTimeInfo *time)
{
	if((offset == DIR_CREATION_TIME_OFFSET)||(offset == DIR_WRITE_TIME_OFFSET))
	{
		unsigned int temp = abstractLittleEndianTo16Bits(entry+offset);
		(*time).hour=((temp&DIR_TIME_HOUR_BIT_MASK)>>DIR_TIME_HOUR_BIT_OFFSET);
		(*time).minute=((temp&DIR_TIME_MINUTE_BIT_MASK)>>DIR_TIME_MINUTE_BIT_OFFSET);
		(*time).second=((temp&DIR_TIME_SECOND_BIT_MASK)>>DIR_TIME_SECOND_BIT_OFFSET);
	}
	else
	{
		(*time).hour=0;
		(*time).minute=0;
		(*time).second=0;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void dirDateAndTimeInfoParseFromDirectoryEntry(char *entry, directoryStructure *p)
{
	dirDateInfoParseFromDirectoryEntry(entry, DIR_CREATION_DATE_OFFSET, &((*p).createDateInfo.date));
	dirDateInfoParseFromDirectoryEntry(entry, DIR_LAST_ACCESS_DATE_OFFSET, &((*p).writeDateInfo.date));
	dirDateInfoParseFromDirectoryEntry(entry, DIR_WRITE_DATE_OFFSET, &((*p).lastestAccessDate));

	dirTimeInfoParseFromDirectoryEntry(entry, DIR_CREATION_TIME_OFFSET, &((*p).createDateInfo.time));
	dirTimeInfoParseFromDirectoryEntry(entry, DIR_WRITE_TIME_OFFSET, &((*p).writeDateInfo.time));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int dirDateInfoTransformTo16Bits(dirDateInfo *date)
{
	return ((((unsigned int)(*date).year)-DIR_DATE_YEAR_START)<<DIR_DATE_YEAR_BIT_OFFSET)|(((unsigned int)(*date).month)<<DIR_DATE_MONTH_BIT_OFFSET)|(((unsigned int)(*date).day)<<DIR_DATE_DAY_BIT_OFFSET);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int dirTimeInfoTransformTo16Bits(dirTimeInfo *time)
{
	return (((unsigned int)(*time).hour)<<DIR_TIME_HOUR_BIT_OFFSET)|(((unsigned int)(*time).minute)<<DIR_TIME_MINUTE_BIT_OFFSET)|(((unsigned int)(*time).second)<<DIR_TIME_SECOND_BIT_OFFSET);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void dateInfoConvertToDirectoryEntry(char *entry, unsigned offset, dirDateInfo *date)
{
	switch(offset)
	{
			case DIR_CREATION_DATE_OFFSET:
			case DIR_LAST_ACCESS_DATE_OFFSET:
			case DIR_WRITE_DATE_OFFSET:
				parsing16BitsToLittleEndian(entry+offset, dirDateInfoTransformTo16Bits(date));
				break;
			default:
				parsing16BitsToLittleEndian(entry+offset, 0x3C21);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void timeInfoConvertToDirectoryEntry(char *entry, unsigned offset, dirTimeInfo *time)
{
	switch(offset)
	{
		case DIR_CREATION_TIME_OFFSET:
		case DIR_WRITE_TIME_OFFSET:
			parsing16BitsToLittleEndian(entry+offset, dirTimeInfoTransformTo16Bits(time));
			break;
		default:
			parsing16BitsToLittleEndian(entry+offset, 0);
	}
}
