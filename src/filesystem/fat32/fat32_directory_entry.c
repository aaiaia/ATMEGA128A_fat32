void dirShortNameChangeCapitalToSmalll(char *str, unsigned char length)
{
	char *p;
	//for(p = str; p<(str+length); p++)
	for(p = str; p<(str+length); p++)
	{
		if(('A'<=(*p))&&((*p)<='Z'))
		{
			(*p)+=('a'-'A');
		}
		else if((*p)==0) break;
	}
	for(;p<(str+length);p++) (*p)=0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void dirShortNameChangeSmallToCapital(char *str, unsigned char length)
{
	char *p;
	//for(p = str; p<(str+length); p++)
	for(p = str; p<(str+length); p++)
	{
		if(('a'<=(*p))&&((*p)<='z'))
		{
			(*p)-=('a'-'A');
		}
		else if((*p)==0) break;
	}
	for(;p<(str+length);p++) (*p)=0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void convertZeroToSpace(char *p, unsigned char length)
{
	unsigned char i;
	for(i=0; i<length; i++)
	{
		if( *(p+i) == 0 )
		{
			*(p+i)=DIR_NAME_EMPTY_DATA;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void convertSpaceToZero(char *p, unsigned char length)
{
	unsigned char i;
	for(i=0; i<length; i++)
	{
		if( *(p+i) ==  DIR_NAME_EMPTY_DATA)
		{
			*(p+i)=0;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void checkAndConvertSimpleNameStringForm(char *p, unsigned char length)
{
	unsigned char i;
	for(i=0; i<length; i++)
	{
		if(('A'<=*(p+i))&&(*(p+i)<='Z'))
		{
			*(p+i)+=('a'-'A');
		}
		else if( *(p+i) == DIR_NAME_EMPTY_DATA )
		{
			*(p+i)=0;
		}
	}
	*(p+length)=0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char createOneRandomChar(char *oneChar)
{
	initInnerAdcConverter();
	if((getInnerAdcValue(7)%2) == 0)	return ((( *(oneChar+0)^getInnerAdcValue(7)^*(oneChar+(getInnerAdcValue(7)%8))^getInnerAdcValue(7)^getInnerAdcValue(7) )%(('Z'+1)-'A') )+'A');
	else								return ((( *(oneChar+1)^getInnerAdcValue(7)^*(oneChar+(getInnerAdcValue(7)%8))^getInnerAdcValue(7)^getInnerAdcValue(7) )%(('9'+1)-'0') )+'0');
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char createSimpleName(char *simpleName)//Use when simplename is exceed 8 character.
{
	if(*(simpleName+DIR_LONG_NAME_MARKER_OFFSET)!=DIR_LONG_NAME_MARKER) return -1;
	initInnerAdcConverter();//make random number.

	if((getInnerAdcValue(7)%2) == 0)	*(simpleName+2)=((( *(simpleName+0)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7)^getInnerAdcValue(7) )%(('Z'+1)-'A') )+'A');
	else								*(simpleName+2)=((( *(simpleName+1)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7)^getInnerAdcValue(7) )%(('9'+1)-'0') )+'0');
	if((getInnerAdcValue(7)%2) == 0)	*(simpleName+3)=((( *(simpleName+2)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7)^getInnerAdcValue(7) )%(('Z'+1)-'A') )+'A');
	else								*(simpleName+3)=((( *(simpleName+3)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7)^getInnerAdcValue(7) )%(('9'+1)-'0') )+'0');
	if((getInnerAdcValue(7)%2) == 0)	*(simpleName+4)=((( *(simpleName+4)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7)^getInnerAdcValue(7) )%(('Z'+1)-'A') )+'A');
	else								*(simpleName+4)=((( *(simpleName+5)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7)^getInnerAdcValue(7) )%(('9'+1)-'0') )+'0');
	if((getInnerAdcValue(7)%2) == 0)	*(simpleName+5)=((( *(simpleName+6)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7)^getInnerAdcValue(7) )%(('Z'+1)-'A') )+'A');
	else								*(simpleName+5)=((( *(simpleName+7)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7)^getInnerAdcValue(7) )%(('9'+1)-'0') )+'0');
	*(simpleName+DIR_LONG_NAME_MARKER_OFFSET)=DIR_LONG_NAME_MARKER;
	*(simpleName+7)='1';

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char createRandomSimpleName(char *simpleName)//Use when simple name is not exceed 8 character.
{
	initInnerAdcConverter();//make random number.

	if((getInnerAdcValue(7)%2) == 0)	*(simpleName+0)=((( *(simpleName+0)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('Z'+1)-'A') )+'A');
	else								*(simpleName+0)=((( *(simpleName+1)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('9'+1)-'0') )+'0');
	if((getInnerAdcValue(7)%2) == 0)	*(simpleName+1)=((( *(simpleName+0)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('Z'+1)-'A') )+'A');
	else								*(simpleName+1)=((( *(simpleName+1)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('9'+1)-'0') )+'0');
	if((getInnerAdcValue(7)%2) == 0)	*(simpleName+2)=((( *(simpleName+0)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('Z'+1)-'A') )+'A');
	else								*(simpleName+2)=((( *(simpleName+1)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('9'+1)-'0') )+'0');
	if((getInnerAdcValue(7)%2) == 0)	*(simpleName+3)=((( *(simpleName+2)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('Z'+1)-'A') )+'A');
	else								*(simpleName+3)=((( *(simpleName+3)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('9'+1)-'0') )+'0');
	if((getInnerAdcValue(7)%2) == 0)	*(simpleName+4)=((( *(simpleName+4)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('Z'+1)-'A') )+'A');
	else								*(simpleName+4)=((( *(simpleName+5)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('9'+1)-'0') )+'0');
	if((getInnerAdcValue(7)%2) == 0)	*(simpleName+5)=((( *(simpleName+6)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('Z'+1)-'A') )+'A');
	else								*(simpleName+5)=((( *(simpleName+7)^getInnerAdcValue(7)^*(simpleName+(getInnerAdcValue(7)%8))^getInnerAdcValue(7) )%(('9'+1)-'0') )+'0');
	*(simpleName+DIR_LONG_NAME_MARKER_OFFSET)=DIR_LONG_NAME_MARKER;
	*(simpleName+7)='1';

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char setDirLongNameEntryInfomation(directoryAndFileEntryInformation *p)
{
	if( *((*p).dirStructure.dirName.simple+DIR_SIMPLE_NAME_MAXIMUM_LENGTH-2) != '~' )
	{
		(*p).entryInfo.extensionNameEntryCount=0;
		return -1;
	}
	(*p).entryInfo.extensionNameEntryCount = (strlen((*p).dirStructure.dirName.fullName)/LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY);
	if( (strlen((*p).dirStructure.dirName.fullName)%LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY) != 0)
	{
		(*p).entryInfo.extensionNameEntryCount++;
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char setDirBasicInfomation(directoryStructure *p, char *name, unsigned char attribute, unsigned long fileSize)
{
	if(p == 0) return -1;
//name is long?... or not alphabet?
//directory? or file?
//	unsigned char flag=0;//1 is long name. case is long name, long extension and both string is long.


	unsigned int nameLength;
	for(nameLength=0; ((*(name+nameLength)!=0)&&(nameLength<LONG_NAME_MAXIMUM_LENGTH)); nameLength++)
	{
		*((*p).dirName.fullName+nameLength)=*(name+nameLength);
	}
	*((*p).dirName.fullName+nameLength)=0;

	if(LONG_NAME_MAXIMUM_LENGTH<nameLength) return -2;
	else if(nameLength == 0) return -1;
	// else if(attribute == 0) return -3;


	unsigned char extensionNameLength = 0;
	char *extensionNamePointer = name+nameLength;//pointer is indicate last location of string.
	//unsigned char i;

	if( *((*p).dirName.fullName) != '.' )
	{
		//if not input attributes this part make ploblem that string processing, so want to edit name, attribute must have any value that archive or directory.
		for(extensionNamePointer = (name + nameLength - 1); ((*extensionNamePointer != '.')&&(name<extensionNamePointer)); extensionNamePointer--);//find dot location

		if(extensionNamePointer != name)//exist extension String
		{
			//To get full name length, that have dot and extension name, using strlen(name).
			nameLength = extensionNamePointer - name ;//name Length is minus extension name length

			extensionNamePointer++;//move from '.' to any one word(at gif, to 'g')
			extensionNameLength = strlen(extensionNamePointer);//set extension string length
			/* if you have to get full name length after this line, nameLength+1+extensionNameLength is full name length... */
		}
	}

	if(nameLength > DIR_SIMPLE_NAME_MAXIMUM_LENGTH)//if name length is exceed over 8 character, created name is long name.
	{
		/*simple Name and extension string copy...*/
		strncpy((*p).dirName.simple, name, DIR_SIMPLE_NAME_MAXIMUM_LENGTH);
		*((*p).dirName.simple + DIR_SIMPLE_NAME_MAXIMUM_LENGTH) = 0;
		*((*p).dirName.simple + DIR_SIMPLE_NAME_MAXIMUM_LENGTH-2) = '~';
		*((*p).dirName.simple + DIR_SIMPLE_NAME_MAXIMUM_LENGTH-1) = '1';
	}
	else//created name is simple name.
	{
		/*simple Name and extension string copy...*/
		// strcpy((*p).dirName.simple, name, nameLength);
		strcpy((*p).dirName.simple, name);
	}


	if(extensionNameLength > DIR_EXTENSION_MAXUMUM_LENGTH)//if extemtion length is exceed over 3 character,created name is long name.
	{
		strncpy((*p).dirName.extension, extensionNamePointer, DIR_EXTENSION_MAXUMUM_LENGTH);
		*((*p).dirName.extension+DIR_EXTENSION_MAXUMUM_LENGTH) = 0;

		if( *((*p).dirName.simple+ DIR_SIMPLE_NAME_MAXIMUM_LENGTH-2) != '~' )//simple name is not exceed 8 char. But, extension name is exceed 3 character. So. random simple name is need...
		{
			createSimpleName((*p).dirName.simple);
		}
	}
	else if(extensionNameLength == 0)
	{
		*((*p).dirName.extension)=0;
	}
	else
	{
		// strncpy((*p).dirName.extension, extensionNamePointer, extensionNameLength);
		strcpy((*p).dirName.extension, extensionNamePointer);
	}

//	dirShortNameChangeSmallToCapital((*p).dirName.simple, strlen((*p).dirName.simple));
//	dirShortNameChangeSmallToCapital((*p).dirName.extension, extensionNameLength);
	dirShortNameChangeSmallToCapital((*p).dirName.simple, DIR_SIMPLE_NAME_MAXIMUM_LENGTH);
	dirShortNameChangeSmallToCapital((*p).dirName.extension, DIR_EXTENSION_MAXUMUM_LENGTH);

	(*p).otherInfo.attribute = attribute;/*set attribute*/
	(*p).otherInfo.fileSize = fileSize;/*set file size*/

	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*When long Name check-sum can use, simple and extension name are decide.*/
unsigned char generateLongNameCheckSum(char *simple, char *extension)
{
	unsigned char simpleAndExtensionLength=0;
	unsigned char sum=0;

	simpleAndExtensionLength=(DIR_SIMPLE_NAME_MAXIMUM_LENGTH+DIR_EXTENSION_MAXUMUM_LENGTH);

	while(simpleAndExtensionLength!=3)
	{
		if(*simple !=0)
		{
			sum=((sum&1)?0x80:0) + (sum>>1) + *simple++;
			simpleAndExtensionLength--;
		}
		else
		{

			sum=((sum&1)?0x80:0) + (sum>>1) + 0x20;
			simple++;
			simpleAndExtensionLength--;
		}
	}

	while(simpleAndExtensionLength!=0)
	{
		if(*extension !=0)
		{
			sum=((sum&1)?0x80:0) + (sum>>1) + *extension++;
			simpleAndExtensionLength--;
		}
		else
		{

			sum=((sum&1)?0x80:0) + (sum>>1) + 0x20;
			extension++;
			simpleAndExtensionLength--;
		}
	}
	return sum;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char findEmptyDirEntry(fat32Info *diskInfo, clustorData *searchingSecterBuffer, CLUSTOR_LOCATION targetClustor, unsigned char totalOccupiedEntryNumber, entryLocationInfomation *dirEntryInfo)
{
	if(totalOccupiedEntryNumber == 0) return -1;

	char *str;
	unsigned char foundSeriesEntryNumber=0;

	(*searchingSecterBuffer).secterInClustor=0;
	(*searchingSecterBuffer).locatedClustor=targetClustor;

								// sprintf(g_strBuf.dat, "T.O.E.N:%d ", totalOccupiedEntryNumber);
								// sendString(g_strBuf.dat);

	checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);

	do
	{
		if( !((*searchingSecterBuffer).secterInClustor < ((*diskInfo).secterPerClustor)) )//finding secter is lastest secter of clustor?
		{//lastest secter of clustor. loading next clustor
			(*searchingSecterBuffer).locatedClustor=(*searchingSecterBuffer).nextClustor;
			checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);

			(*searchingSecterBuffer).secterInClustor=0;
		}


		readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);

		for(str=(*searchingSecterBuffer).secterData.data; str<(*searchingSecterBuffer).secterData.data+SD_DATA_BUFFER_SIZE; str+=DIR_DISCRIPTION_LENGTH)
		{
			if( ((*str) == DIR_DELEDTED) || ((*str) == DIR_EMPTY))
			{
				//(*dirEntryInfo).extensionNameEntryCount = 0;//external dir Entry reset.//2015.01.13-1
				if(foundSeriesEntryNumber == 0)//lastest entry was back up
				{
						(*dirEntryInfo).location.clustor=(*searchingSecterBuffer).locatedClustor;
						(*dirEntryInfo).location.secterInClustor=(*searchingSecterBuffer).secterInClustor;

						(*dirEntryInfo).entryNumberOrOffset=(str-(*searchingSecterBuffer).secterData.data);
				}
				foundSeriesEntryNumber++;
			}
			else
			{
				foundSeriesEntryNumber=0;
			}

			if(totalOccupiedEntryNumber <= foundSeriesEntryNumber)
			{
				//(*dirEntryInfo).extensionNameEntryCount = (totalOccupiedEntryNumber-1);
				return 0;
			}
		}
		(*searchingSecterBuffer).secterInClustor++;
	}
	while( ((*searchingSecterBuffer).nextClustor != CLUSTOR_IS_END) || ((*searchingSecterBuffer).secterInClustor<((*diskInfo).secterPerClustor)) );
	(*dirEntryInfo).location.clustor = (*searchingSecterBuffer).locatedClustor;
	(*dirEntryInfo).location.secterInClustor = ((*diskInfo).secterPerClustor-1);

	return -1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setTargetLocation(logicalLocationInfomation *p, CLUSTOR_LOCATION clustor, unsigned char secterInClustor)
{
	(*p).clustor = clustor;
	(*p).secterInClustor = secterInClustor;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
to using findTargetFileDirectoryEntryUsingDirectionClustor
1. set directoryAndFileEntryInformation(that is structure).entryInfo.location.clustor//this value is indicate first clustor will be find.
2. set directoryAndFileEntryInformation(that is structure).dirStructure.otherInfo.indicateFirstClustor//this value is target value.
*/
char findDirEntryUsingIndicateClustor(fat32Info *diskInfo, clustorData *searchingSecterInClustor, directoryAndFileEntryInformation *physicalDirLocationInfo)
{
	/////////////////////////////////*using findTargetFileDirectoryEntryUsingDirectionClustor start*/////////////////////////////////
	//(*findDirEntry).dirStructure.otherInfo.indicateFirstClustor=0x49;/*setting target file located Clustor*/
	//(*findDirEntry).entryInfo.location.clustor=sdCardInfo.rootClustor;//(*physicalDirLocationInfo).entryInfo.location.clustor
	//
	//findDirEntryUsingIndicateClustor(&sdCardInfo, &clustor, findDirEntry);
/*
	(*findDirEntry).entryInfo.location.clustor=sdCardInfo.rootClustor;
	sprintf((*findDirEntry).dirStructure.dirName.fullName, "middleNameSearching");
	findDirEntryUsingName(&sdCardInfo, &clustor, findDirEntry);



	resetGlcd();
	sprintf(g_glcdBuf, "indicate 0x%lx", (*findDirEntry).dirStructure.otherInfo.indicateFirstClustor);
	putStringInGlcdAtPage(PAGE1, g_glcdBuf);

	sprintf(g_glcdBuf, "Entry No 0x%x", (*findDirEntry).entryInfo.entryNumberOrOffset);
	putStringInGlcdAtPage(PAGE2, g_glcdBuf);

	sprintf(g_glcdBuf, "clustor 0x%lx", (*findDirEntry).entryInfo.location.clustor);
	putStringInGlcdAtPage(PAGE3, g_glcdBuf);

	sprintf(g_glcdBuf, "secterInC. 0d%d", (*findDirEntry).entryInfo.location.secterInClustor);
	putStringInGlcdAtPage(PAGE4, g_glcdBuf);

	sprintf(g_glcdBuf, "size 0x%lx", (*findDirEntry).dirStructure.otherInfo.fileSize);
	putStringInGlcdAtPage(PAGE5, g_glcdBuf);

	sprintf(g_glcdBuf, "attribute 0x%x", (*findDirEntry).dirStructure.otherInfo.attribute);
	putStringInGlcdAtPage(PAGE6, g_glcdBuf);

	sprintf(g_glcdBuf ,"%s", stringTemp);
	putStringInGlcdAtPage(PAGE7, g_glcdBuf);
																					nextSequence();

*/
	/////////////////////////////////*using findTargetFileDirectoryEntryUsingDirectionClustor end*/////////////////////////////////

	if( !((*physicalDirLocationInfo).entryInfo.location.secterInClustor < ((*diskInfo).secterPerClustor)) )
	{
		(*physicalDirLocationInfo).entryInfo.location.secterInClustor=0;
	}

	unsigned char count=0;
	char *str;
	CLUSTOR_LOCATION abstractFirstClustor = 0;

	if( !((*physicalDirLocationInfo).entryInfo.location.secterInClustor < ((*diskInfo).secterPerClustor)) )
	{
		(*physicalDirLocationInfo).entryInfo.location.secterInClustor=0;
	}

	(*searchingSecterInClustor).locatedClustor=(*physicalDirLocationInfo).entryInfo.location.clustor;
	(*searchingSecterInClustor).secterInClustor=(*physicalDirLocationInfo).entryInfo.location.secterInClustor;

	//reset long name entry info
	(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount=0;


	checkFatAndLocatNextClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor);//if want check wrong fat table, added exception process

	do
	{
		if( !((*searchingSecterInClustor).secterInClustor < (*diskInfo).secterPerClustor) )//finding secter is lastest secter of clustor?
		{//lastest secter of clustor. loading next clustor
			(*searchingSecterInClustor).locatedClustor=(*searchingSecterInClustor).nextClustor;
			checkFatAndLocatNextClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor);
			(*searchingSecterInClustor).secterInClustor=0;
		}

		readSecterInClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor, (*searchingSecterInClustor).secterInClustor);


		for(str=(*searchingSecterInClustor).secterData.data; str<(*searchingSecterInClustor).secterData.data+SD_DATA_BUFFER_SIZE; str+=DIR_DISCRIPTION_LENGTH)
		{
		/*
		this function is not work that abstract simple and full name.
		compare findTargetFileDirectoryEntryUsingName after added code.
		*/
			if((*str==DIR_DELEDTED)||(*str==DIR_EMPTY))
			{
				(*physicalDirLocationInfo).entryInfo.longNameLocation.clustor=0;
				(*physicalDirLocationInfo).entryInfo.longNameLocation.secterInClustor=-1;
				(*physicalDirLocationInfo).entryInfo.longNameEntryOffset=-1;

				(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount=0;
				continue;
			}
			else if(*(str+DIR_ATTR_OFFSET)==ATTR_LONG_NAME)
			{
				if( ( (*(str)) & LONG_NAME_LASTEST_MASK ) == LONG_NAME_LASTEST_VALID_VALUE )//save first long name entry location info.
				{
					(*physicalDirLocationInfo).entryInfo.longNameLocation.clustor=(*searchingSecterInClustor).locatedClustor;
					(*physicalDirLocationInfo).entryInfo.longNameLocation.secterInClustor=(*searchingSecterInClustor).secterInClustor;
					(*physicalDirLocationInfo).entryInfo.longNameEntryOffset=str-(*searchingSecterInClustor).secterData.data;

					(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount=((*str)&LONG_NAME_NUMBER_MASK);
				}

				continue;
			}

			/*Long Name entry is not filtered??....???!?!?!?*/
			abstractFirstClustor = abstractLittleEndianTo16Bits(str+LSB_FIRST_CLUSTOR_OFFSET);
			abstractFirstClustor |= (((unsigned long)abstractLittleEndianTo16Bits(str+MSB_FIRST_CLUSTOR_OFFSET))<<16);
			if((*physicalDirLocationInfo).dirStructure.otherInfo.indicateFirstClustor==abstractFirstClustor)
			{
				/*!!!!if varing lastest access time, added function that abstract lastest access time from dir entry, this location...*/
				(*physicalDirLocationInfo).entryInfo.entryNumberOrOffset=str-(*searchingSecterInClustor).secterData.data;

				(*physicalDirLocationInfo).entryInfo.location.clustor=(*searchingSecterInClustor).locatedClustor;
				(*physicalDirLocationInfo).entryInfo.location.secterInClustor=(*searchingSecterInClustor).secterInClustor;

				dirOtherInfoAbstractFromDirectoryEntry(str, &((*physicalDirLocationInfo).dirStructure.otherInfo));

				dirDateAndTimeInfoParseFromDirectoryEntry(str, &((*physicalDirLocationInfo).dirStructure));
				return 0;
			}

			///////reset longName entry info.
			(*physicalDirLocationInfo).entryInfo.longNameLocation.clustor=0;
			(*physicalDirLocationInfo).entryInfo.longNameLocation.secterInClustor=-1;
			(*physicalDirLocationInfo).entryInfo.longNameEntryOffset=-1;

			(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount=0;
		}

		(*searchingSecterInClustor).secterInClustor++;
	}

	while( ((*searchingSecterInClustor).nextClustor != CLUSTOR_IS_END) || ((*searchingSecterInClustor).secterInClustor<((*diskInfo).secterPerClustor)) );
	(*physicalDirLocationInfo).entryInfo.location.secterInClustor=(*diskInfo).secterPerClustor-1;

	//if target is not found, (*physicalDirLocationInfo).entryInfo.location.clustor is indicate lastest clustor is found by this function.

		//reset long name entry info
		(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount=0;

		(*physicalDirLocationInfo).entryInfo.longNameLocation.clustor=0;
		(*physicalDirLocationInfo).entryInfo.longNameLocation.secterInClustor=-1;
		(*physicalDirLocationInfo).entryInfo.longNameEntryOffset=-1;
	return -1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char findDirEntryUsingSimpleName(fat32Info *diskInfo, clustorData *searchingSecterBuffer, directoryAndFileEntryInformation *physicalDirLocationInfo)
{
	if( *((*physicalDirLocationInfo).dirStructure.dirName.simple)==0) return -1;

	if( !((*physicalDirLocationInfo).entryInfo.location.secterInClustor < ((*diskInfo).secterPerClustor)) )
	{
		(*physicalDirLocationInfo).entryInfo.location.secterInClustor=0;
	}

	unsigned char longNameEntryCount=0;

	unsigned char i;
	char *str;

	(*searchingSecterBuffer).locatedClustor=(*physicalDirLocationInfo).entryInfo.location.clustor;
	(*searchingSecterBuffer).secterInClustor=0;


	checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);

	do
	{
		if( !((*searchingSecterBuffer).secterInClustor < (*diskInfo).secterPerClustor) )//finding secter is lastest secter of clustor?
		{//lastest secter of clustor. loading next clustor
			(*searchingSecterBuffer).locatedClustor=(*searchingSecterBuffer).nextClustor;
			checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);

			(*searchingSecterBuffer).secterInClustor=0;
		}

		readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);

		for(str=(*searchingSecterBuffer).secterData.data; str<(*searchingSecterBuffer).secterData.data+SD_DATA_BUFFER_SIZE; str+=DIR_DISCRIPTION_LENGTH)
		{
			if((*str==DIR_DELEDTED)||(*str==DIR_EMPTY))
			{
				//reset long name entry info
				longNameEntryCount=0;
				continue;
			}
			/*In find simple name find, Longname is skipped to fast, but in long name find comparing subfunction increase reserve section in full name when long name is incorrect.*/
			else if(*((*physicalDirLocationInfo).dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH)!=0)
			{
				if((*((*physicalDirLocationInfo).dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH))==1)
				{
					//reset long name entry info
					longNameEntryCount=0;
				}
				(*((*physicalDirLocationInfo).dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH))--;
				continue;
			}
			else if( ( (*(str+DIR_ATTR_OFFSET)) & ATTRIBUTE_MASK ) == ATTRIBUTE_MASK )//encountered long name entry.
			{
				if( ( (*(str)) & LONG_NAME_LASTEST_MASK ) == LONG_NAME_LASTEST_VALID_VALUE )//save first long name entry location info.
				{
					(*physicalDirLocationInfo).entryInfo.longNameLocation.clustor=(*searchingSecterBuffer).locatedClustor;
					(*physicalDirLocationInfo).entryInfo.longNameLocation.secterInClustor=(*searchingSecterBuffer).secterInClustor;
					(*physicalDirLocationInfo).entryInfo.longNameEntryOffset=str-(*searchingSecterBuffer).secterData.data;

					longNameEntryCount=(*(str)&LONG_NAME_NUMBER_MASK);

					*((*physicalDirLocationInfo).dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH)=(*(str)&LONG_NAME_NUMBER_MASK);
				}
				continue;
			}

			for(i=0; i<DIR_SIMPLE_NAME_MAXIMUM_LENGTH+DIR_EXTENSION_MAXUMUM_LENGTH; i++)
			{
				if(i<DIR_SIMPLE_NAME_MAXIMUM_LENGTH)
				{
					if(*(str+i) == *((*physicalDirLocationInfo).dirStructure.dirName.simple+i)) continue;
					else if( (*((*physicalDirLocationInfo).dirStructure.dirName.simple+i) == 0) && (*(str+i)  == DIR_NAME_EMPTY_DATA) ) continue;
					break;
				}
				else
				{
					if(*(str+i) == *((*physicalDirLocationInfo).dirStructure.dirName.extension+i-DIR_SIMPLE_NAME_MAXIMUM_LENGTH)) continue;
					else if( (*((*physicalDirLocationInfo).dirStructure.dirName.extension+i-DIR_SIMPLE_NAME_MAXIMUM_LENGTH) == 0) && (*(str+i)  == DIR_NAME_EMPTY_DATA) ) continue;
					break;
				}
			}

			if(i==(DIR_SIMPLE_NAME_MAXIMUM_LENGTH+DIR_EXTENSION_MAXUMUM_LENGTH))
			{
				/*!!!!if varing lastest access time, added function that abstract lastest access time from dir entry, this location...*/
				dirOtherInfoAbstractFromDirectoryEntry(str, &((*physicalDirLocationInfo).dirStructure.otherInfo));

				dirDateAndTimeInfoParseFromDirectoryEntry(str, &((*physicalDirLocationInfo).dirStructure));

				(*physicalDirLocationInfo).entryInfo.entryNumberOrOffset=str-(*searchingSecterBuffer).secterData.data;

				(*physicalDirLocationInfo).entryInfo.location.clustor=(*searchingSecterBuffer).locatedClustor;
				(*physicalDirLocationInfo).entryInfo.location.secterInClustor=(*searchingSecterBuffer).secterInClustor;


				(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount=longNameEntryCount;
				return 0;
			}
			//reset long name entry info
			longNameEntryCount=0;
		}
		(*searchingSecterBuffer).secterInClustor++;
	}
	while( ((*searchingSecterBuffer).nextClustor != CLUSTOR_IS_END) || ((*searchingSecterBuffer).secterInClustor<((*diskInfo).secterPerClustor)) );
	(*physicalDirLocationInfo).entryInfo.location.secterInClustor=(*diskInfo).secterPerClustor-1;

	//if target location is not found, (*physicalDirLocationInfo).entryInfo.location.clustor is indicate that lastest clustor, that is found by this function.
	(*physicalDirLocationInfo).entryInfo.longNameLocation.clustor=0;
	(*physicalDirLocationInfo).entryInfo.longNameLocation.secterInClustor=-1;
	(*physicalDirLocationInfo).entryInfo.longNameEntryOffset=-1;
	return -1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char notExistSameSimpleName(fat32Info *diskInfo, clustorData *searchingSecterBuffer, CLUSTOR_LOCATION targetClustor, directoryName *dirName)
{
	char *stringBuffer = (char*)malloc(sizeof(DIR_SIMPLE_NAME_MAXIMUM_LENGTH+DIR_EXTENSION_MAXUMUM_LENGTH));
	memset(stringBuffer, DIR_NAME_EMPTY_DATA, sizeof(char)*(DIR_SIMPLE_NAME_MAXIMUM_LENGTH+DIR_EXTENSION_MAXUMUM_LENGTH));

	unsigned char i;

	strncpy(stringBuffer, (*dirName).simple, strlen((*dirName).simple));
	strncpy(stringBuffer+DIR_SIMPLE_NAME_MAXIMUM_LENGTH, (*dirName).extension, strlen((*dirName).extension));

	char *str;
	(*searchingSecterBuffer).secterInClustor = 0;
	(*searchingSecterBuffer).locatedClustor = targetClustor;
	checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);

	do
	{
		if( !((*searchingSecterBuffer).secterInClustor < (*diskInfo).secterPerClustor) )//finding secter is lastest secter of clustor?
		{//lastest secter of clustor. loading next clustor
			(*searchingSecterBuffer).locatedClustor=(*searchingSecterBuffer).nextClustor;
			checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);

			(*searchingSecterBuffer).secterInClustor=0;
		}


		readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);

		for(str=(*searchingSecterBuffer).secterData.data; str<(*searchingSecterBuffer).secterData.data+SD_DATA_BUFFER_SIZE; str+=DIR_DISCRIPTION_LENGTH)
		{
			if((*str==DIR_DELEDTED)||(*str==DIR_EMPTY))
			{
				continue;
			}
			else if(*(str+DIR_ATTR_OFFSET)==ATTR_LONG_NAME)
			{
				continue;
			}

			for(i=0; i<(DIR_SIMPLE_NAME_MAXIMUM_LENGTH+DIR_EXTENSION_MAXUMUM_LENGTH); i++)
			{
				if(*(str+i) != *(stringBuffer+i)) break;
			}

			if(i==(DIR_SIMPLE_NAME_MAXIMUM_LENGTH+DIR_EXTENSION_MAXUMUM_LENGTH)) return -1;
		}
		(*searchingSecterBuffer).secterInClustor++;
	}
	while( ((*searchingSecterBuffer).nextClustor != CLUSTOR_IS_END) || ((*searchingSecterBuffer).secterInClustor<((*diskInfo).secterPerClustor)) );


	free(stringBuffer);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*(dirEntry+0x01), *(dirEntry+0x03), *(dirEntry+0x05), *(dirEntry+0x07), *(dirEntry+0x09),  *(dirEntry+0x0e), *(dirEntry+0x10), *(dirEntry+0x12), *(dirEntry+0x14), *(dirEntry+0x16), *(dirEntry+0x18),  *(dirEntry+0x1c), *(dirEntry+0x1e)
//*(longName+((LONG_NAME_ENTRY_MAXIMUM_NUMBER-(((*dirEntry)&LONG_NAME_NUMBER_MASK)-1))*LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY))
char compareLongNameAndOneEntry(char *start, char *end, char **fileName)
{
	// while(start<end)
	// {
		// //comparing char is alphabet?
		// if( (*(*fileName)) != (*(dirEntry+start)) )//until '/0' is same or, end of entry, compare char.
		// {
			// if( ((**fileName)) == 0)/* If compared string is end... */
			// {
				// if( (*(dirEntry+start) != 0x00) && (*(dirEntry+start) != 0xff) )/* If dir entry is not have empty and end bits. */
				// {
										// // sprintf(g_strBuf.dat, "!0x%x", (**fileName));
										// // sendStringOnly(g_strBuf.dat);
										// // sendCharOnly('|');

										// // sprintf(g_strBuf.dat, "!0x%x", *(dirEntry+i));
										// // sendStringOnly(g_strBuf.dat);
										// // sendCharOnly(':');
					// return -1;
				// }
										// // sprintf(g_strBuf.dat, "0x%x", (**fileName));
										// // sendStringOnly(g_strBuf.dat);
										// // sendCharOnly('|');

										// // sprintf(g_strBuf.dat, "0x%x", *(dirEntry+i));
										// // sendStringOnly(g_strBuf.dat);
										// // sendCharOnly(':');
			// }
			// else
			// {
				// return -3;
			// }
		// }

						// // sendCharOnly(*(*fileName));
						// // sendCharOnly('|');

						// // sendCharOnly(*(dirEntry+i));
						// // sendCharOnly(':');
		// /* If compared string is end. */
		// if( ((**fileName)) != 0)
		// {
			// (*fileName)++;
		// }
		// /*Entry character and file name are same.*/
		// i+=LONG_DIR_NAME_ONE_WORD_SIZE;
	// }

	// return 0;
	while(start<end)
	{
		if((*(*fileName))==0)
		{
			if(((*start)!=0x00)&&((*start)!=0xFF))	return -1;
		}
		else if((*start)!=(*((*fileName))))
		{
			return -2;
		}
		else
		{
			(*fileName)++;
		}
		start+=LONG_DIR_NAME_ONE_WORD_SIZE;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char compareLongNameStringAndLongNameDirEntry(char* fileName, char *dirEntry)
{
	if( ((*dirEntry)&LONG_NAME_LASTEST_MASK) == LONG_NAME_LASTEST_VALID_VALUE )
	{
		if( LONG_NAME_ENTRY_MAXIMUM_NUMBER < ((*(dirEntry))&LONG_NAME_NUMBER_MASK) )
		{
					// sprintf(g_strBuf.dat, "%d(0x%x) < %d(0x%x) ", LONG_NAME_ENTRY_MAXIMUM_NUMBER, LONG_NAME_ENTRY_MAXIMUM_NUMBER, ((*(dirEntry))&LONG_NAME_NUMBER_MASK), ((*(dirEntry))&LONG_NAME_NUMBER_MASK));
					// sendStringOnly(g_strBuf.dat);
					// sendString("long name entry exceed.");
			return -1;
		}
	}

	char *comaparePointer = fileName+((((*dirEntry)&LONG_NAME_NUMBER_MASK)-1)*LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY);//set longName pointer to compare position along to long name entry number.

	if( compareLongNameAndOneEntry(dirEntry+LONG_DIR_NAME1_OFFSET, dirEntry+LONG_DIR_NAME1_OFFSET+LONG_DIR_NAME1_SIZE, &comaparePointer) )
	{
					// sendCharOnly('\n');
					// sendCharOnly('\r');
		// *(fileName+LONG_NAME_MAXIMUM_LENGTH) = ((*(dirEntry))&LONG_NAME_NUMBER_MASK);//in full name string, set longname entry number.
		return 1;
	}
	if( compareLongNameAndOneEntry(dirEntry+LONG_DIR_NAME2_OFFSET, dirEntry+LONG_DIR_NAME2_OFFSET+LONG_DIR_NAME2_SIZE, &comaparePointer) )
	{
					// sendCharOnly('\n');
					// sendCharOnly('\r');
		// *(fileName+LONG_NAME_MAXIMUM_LENGTH) = ((*(dirEntry))&LONG_NAME_NUMBER_MASK);//in full name string, set longname entry number.
		return 2;
	}
	if( compareLongNameAndOneEntry(dirEntry+LONG_DIR_NAME3_OFFSET, dirEntry+LONG_DIR_NAME3_OFFSET+LONG_DIR_NAME3_SIZE, &comaparePointer) )
	{
					// sendCharOnly('\n');
					// sendCharOnly('\r');
		// *(fileName+LONG_NAME_MAXIMUM_LENGTH) = ((*(dirEntry))&LONG_NAME_NUMBER_MASK);//in full name string, set longname entry number.
		return 3;
	}
					// sendCharOnly('\n');
					// sendCharOnly('\r');
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char findDirEntryUsingLongName(fat32Info *diskInfo, clustorData *searchingSecterInClustor, directoryAndFileEntryInformation *physicalDirLocationInfo)//indicateFirstClustor
{
	if(strlen((*physicalDirLocationInfo).dirStructure.dirName.fullName) == 0) return -1;
	if(!(0<(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount)) return -2;
	/*to create new dir entry, do not varing value of (*physicalDirLocationInfo).entryInfo.extensionNameEntryCount.*/
	// unsigned char longNameEntryCount=0;

	char *str;
	unsigned char i;

	unsigned char passingEntryNumber=0;

	(*searchingSecterInClustor).locatedClustor=(*physicalDirLocationInfo).entryInfo.location.clustor;
	(*searchingSecterInClustor).secterInClustor=0;


	checkFatAndLocatNextClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor);//if want check wrong fat table, added exception process
	do
	{
		if( !((*searchingSecterInClustor).secterInClustor < (*diskInfo).secterPerClustor) )//finding secter is lastest secter of clustor?
		{//lastest secter of clustor. loading next clustor
			(*searchingSecterInClustor).locatedClustor=(*searchingSecterInClustor).nextClustor;
			checkFatAndLocatNextClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor);
			(*searchingSecterInClustor).secterInClustor=0;
		}


		readSecterInClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor, (*searchingSecterInClustor).secterInClustor);

		for(str=(*searchingSecterInClustor).secterData.data; str<(*searchingSecterInClustor).secterData.data+SD_DATA_BUFFER_SIZE; str+=DIR_DISCRIPTION_LENGTH)
		{
					// if((*str!=DIR_DELEDTED)&&(*str!=DIR_EMPTY))//deleted or empty entry
					// {
						// sprintf(g_strBuf.dat,
						// "passingEntryNum=%d, extEntryNum=%d, longEntryNum=%d, ATTR=%x, VLE=%x",
						// passingEntryNumber, (*physicalDirLocationInfo).entryInfo.extensionNameEntryCount, ((*str)&LONG_NAME_NUMBER_MASK), ( (*(str+DIR_ATTR_OFFSET)) & ATTRIBUTE_MASK ), ((*str)&LONG_NAME_LASTEST_MASK));
						// sendString(g_strBuf.dat);
					// }
			if((*str==DIR_DELEDTED)||(*str==DIR_EMPTY))//deleted or empty entry
			{
				//reset long name entry info
				/*to create new dir entry, do not varing value of (*physicalDirLocationInfo).entryInfo.extensionNameEntryCount.*/
				//longNameEntryCount=0;
				// (*((*physicalDirLocationInfo).dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH))=0;
				passingEntryNumber=0;
				continue;
			}
			/*In find simple name find, Longname is skipped to fast, but in long name find comparing subfunction increase reserve section in full name when long name is incorrect.*/
			// else if(*((*physicalDirLocationInfo).dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH)!=0)
			else if(passingEntryNumber!=0)
			{
				// (*((*physicalDirLocationInfo).dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH))--;
				passingEntryNumber--;
				continue;
			}
			else if( ( (*(str+DIR_ATTR_OFFSET)) & ATTRIBUTE_MASK ) == ATTR_LONG_NAME )//encountered long name entry.
			{
						// sendString("Long name entry detected.");
				if( ( (*(str)) & LONG_NAME_LASTEST_MASK ) == LONG_NAME_LASTEST_VALID_VALUE )//save first long name entry location info.
				{
								// sendString("Last long name entry detected.");
					if( (*physicalDirLocationInfo).entryInfo.extensionNameEntryCount != ( (*(str)) & LONG_NAME_NUMBER_MASK ) )
					{
								// sendString("Long name entry and calculated entry num is different.");
						// longNameEntryCount=0;
						// *((*physicalDirLocationInfo).dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH) = ((*str)&LONG_NAME_NUMBER_MASK);//in full name string, set longname entry number.
						passingEntryNumber=(((*str)&LONG_NAME_NUMBER_MASK));//in full name string, set longname entry number.
						continue;
					}
								// sendString("Long name entry location saved.");
					(*physicalDirLocationInfo).entryInfo.longNameLocation.clustor=(*searchingSecterInClustor).locatedClustor;
					(*physicalDirLocationInfo).entryInfo.longNameLocation.secterInClustor=(*searchingSecterInClustor).secterInClustor;
					(*physicalDirLocationInfo).entryInfo.longNameEntryOffset=str-(*searchingSecterInClustor).secterData.data;

					/*to create new dir entry, do not varing value of (*physicalDirLocationInfo).entryInfo.extensionNameEntryCount.*/
					//longNameEntryCount=(*(str)&LONG_NAME_NUMBER_MASK);
					/* To compare with simple name, abstract check sum bits and loaded directoryAndFileEntryInformation */
					(*physicalDirLocationInfo).entryInfo.extensionNameChkSum=*(str+LONG_DIR_CHECK_SUM_OFFSET);
				}

				if(compareLongNameStringAndLongNameDirEntry((*physicalDirLocationInfo).dirStructure.dirName.fullName, str))
				{
							// sendString("Long name entry is different.");
							// sprintf(g_strBuf.dat ,"%s, ", (*physicalDirLocationInfo).dirStructure.dirName.fullName+(((*str)&LONG_NAME_NUMBER_MASK)-1)*LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY);
							// sprintf(g_strBuf.dat, "%s|%c|%c|%c|%c|%c|", g_strBuf.dat, *(str+1), *(str+3), *(str+5), *(str+7), *(str+8));
					passingEntryNumber=(((*(str))&LONG_NAME_NUMBER_MASK));
					/*reset long name entry info*/
					//longNameEntryCount=0;
				}
				continue;
			}
			// else if(longNameEntryCount==0) continue;
			else if( *(str+DIR_SIMPLE_NAME_MAXIMUM_LENGTH-2) != '~')
			{
				continue;
			}

			/* Alternate below start */
			dirSimpleNameAbstractFromDirectoryEntry(str, &((*physicalDirLocationInfo).dirStructure.dirName));
			/* Alternate above end */

			/*!!!!if varing lastest access time, added function that abstract lastest access time from dir entry, this location...*/
			dirOtherInfoAbstractFromDirectoryEntry(str, &((*physicalDirLocationInfo).dirStructure.otherInfo));

			dirDateAndTimeInfoParseFromDirectoryEntry(str, &((*physicalDirLocationInfo).dirStructure));

			(*physicalDirLocationInfo).entryInfo.entryNumberOrOffset=str-(*searchingSecterInClustor).secterData.data;

			(*physicalDirLocationInfo).entryInfo.location.clustor=(*searchingSecterInClustor).locatedClustor;
			(*physicalDirLocationInfo).entryInfo.location.secterInClustor=(*searchingSecterInClustor).secterInClustor;

			/*to create new dir entry, do not varing value of (*physicalDirLocationInfo).entryInfo.extensionNameEntryCount.*/
			//(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount=longNameEntryCount;

			return 0;
		}
		(*searchingSecterInClustor).secterInClustor++;
	}
	while( ((*searchingSecterInClustor).nextClustor != CLUSTOR_IS_END) || ((*searchingSecterInClustor).secterInClustor<((*diskInfo).secterPerClustor)) );
	(*physicalDirLocationInfo).entryInfo.location.secterInClustor=(*diskInfo).secterPerClustor-1;
	//if target is not found, (*physicalDirLocationInfo).entryInfo.location.clustor is indicate lastest clustor, that is found by this function.

		//reset long name entry info
		/*to create new dir entry, do not varing value of (*physicalDirLocationInfo).entryInfo.extensionNameEntryCount.*/
		(*physicalDirLocationInfo).entryInfo.longNameLocation.clustor=0;
		(*physicalDirLocationInfo).entryInfo.longNameLocation.secterInClustor=-1;
		(*physicalDirLocationInfo).entryInfo.longNameEntryOffset=-1;
	return -1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char notExistSameLongName(fat32Info *diskInfo, clustorData *searchingSecterInClustor, CLUSTOR_LOCATION targetClustor, directoryName *dirName)
{
	/*to create new dir entry, do not varing value of (*physicalDirLocationInfo).entryInfo.extensionNameEntryCount.*/
	unsigned char occpiedLongNameEntryNumber;
	unsigned char i;
	if((i=strlen((*dirName).fullName)) == 0) return -1;

	occpiedLongNameEntryNumber = (i/LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY);
	if( (i%LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY) != 0)
	{
		occpiedLongNameEntryNumber++;
	}

	unsigned char passingEntryNumber=0;

	char *str;

	(*searchingSecterInClustor).locatedClustor=targetClustor;
	(*searchingSecterInClustor).secterInClustor=0;


	checkFatAndLocatNextClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor);//if want check wrong fat table, added exception process

	do
	{
		if( !((*searchingSecterInClustor).secterInClustor < (*diskInfo).secterPerClustor) )//finding secter is lastest secter of clustor?
		{//lastest secter of clustor. loading next clustor
			(*searchingSecterInClustor).locatedClustor=(*searchingSecterInClustor).nextClustor;
			checkFatAndLocatNextClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor);
			(*searchingSecterInClustor).secterInClustor=0;
		}


		readSecterInClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor, (*searchingSecterInClustor).secterInClustor);

		for(str=(*searchingSecterInClustor).secterData.data; str<(*searchingSecterInClustor).secterData.data+SD_DATA_BUFFER_SIZE; str+=DIR_DISCRIPTION_LENGTH)
		{
			if((*str==DIR_DELEDTED)||(*str==DIR_EMPTY))//deleted or empty entry
			{
				passingEntryNumber=0;
				continue;
			}
			else if(passingEntryNumber!=0)
			{
				passingEntryNumber--;
				continue;
			}
			else if( ( (*(str+DIR_ATTR_OFFSET)) & ATTRIBUTE_MASK ) == ATTR_LONG_NAME )//encountered long name entry.
			{
				if( ( (*(str)) & LONG_NAME_LASTEST_MASK ) == LONG_NAME_LASTEST_VALID_VALUE )//save first long name entry location info.
				{
					if( occpiedLongNameEntryNumber != ( (*(str)) & LONG_NAME_NUMBER_MASK ) )
					{
						passingEntryNumber=((*(str)&LONG_NAME_NUMBER_MASK));//in full name string, set longname entry number.
						continue;
					}
				}

				if(compareLongNameStringAndLongNameDirEntry((*dirName).fullName, str))
				{
					passingEntryNumber=(((*(str))&LONG_NAME_NUMBER_MASK));
				}
				continue;
			}
			else if( *(str+DIR_SIMPLE_NAME_MAXIMUM_LENGTH-2) != '~')
			{
				continue;
			}

			return -1;
		}
		(*searchingSecterInClustor).secterInClustor++;
	}
	while( ((*searchingSecterInClustor).nextClustor != CLUSTOR_IS_END) || ((*searchingSecterInClustor).secterInClustor<((*diskInfo).secterPerClustor)) );

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*findDirEntryUsingName*//*findDirEntryUsingName*//*findDirEntryUsingName*//*findDirEntryUsingName*//*findDirEntryUsingName*/
//1 3 5 7 9  e 10 12 14 16 18  1c 1e
//char findDirEntryUsingIndicateClustor(fat32Info *diskInfo, clustorData *searchingSecterInClustor, directoryAndFileEntryInformation *physicalDirLocationInfo)
/*
	When use findDirEntryUsingName(), character is must set by setDirBasicInfomation().
*/
/*test Codes*/
/*
	memset(&(fileBrowserData.findEntry), 0x00, sizeof(directoryAndFileEntryInformation));
	resultBuffer=254;

	resultBuffer=findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), sdCardInfo.rootClustor, "dslsd6fg");


																					displayDirectoryAndFileEntryInfomation1(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();

																					displayDirectoryAndFileEntryInfomation2(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();



	memset(&(fileBrowserData.findEntry), 0x00, sizeof(directoryAndFileEntryInformation));
	resultBuffer=254;

	resultBuffer=findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), sdCardInfo.rootClustor, "mvdo9q5e4q6rw.txt");


																					displayDirectoryAndFileEntryInfomation1(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();

																					displayDirectoryAndFileEntryInfomation2(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();




	memset(&(fileBrowserData.findEntry), 0x00, sizeof(directoryAndFileEntryInformation));
	resultBuffer=254;

	resultBuffer=findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), sdCardInfo.rootClustor, "test123hgf43dsf5002.txt");


																					displayDirectoryAndFileEntryInfomation1(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();

																					displayDirectoryAndFileEntryInfomation2(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();




	memset(&(fileBrowserData.findEntry), 0x00, sizeof(directoryAndFileEntryInformation));
	resultBuffer=254;

	resultBuffer=findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), sdCardInfo.rootClustor, "fdjf013.txt");


																					displayDirectoryAndFileEntryInfomation1(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();

																					displayDirectoryAndFileEntryInfomation2(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();



	memset(&(fileBrowserData.findEntry), 0x00, sizeof(directoryAndFileEntryInformation));
	resultBuffer=254;

	resultBuffer=findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), sdCardInfo.rootClustor, "keiwkwjerlkdjwier4h60.txt");


																					displayDirectoryAndFileEntryInfomation1(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();

																					displayDirectoryAndFileEntryInfomation2(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();



	memset(&(fileBrowserData.findEntry), 0x00, sizeof(directoryAndFileEntryInformation));
	resultBuffer=254;

	resultBuffer=findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), sdCardInfo.rootClustor, "gklduw374924h60.txt");


																					displayDirectoryAndFileEntryInfomation1(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();

																					displayDirectoryAndFileEntryInfomation2(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();



	memset(&(fileBrowserData.findEntry), 0x00, sizeof(directoryAndFileEntryInformation));
	resultBuffer=254;

	resultBuffer=findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), sdCardInfo.rootClustor, "test123hgf43dsf5012.txt");


																					displayDirectoryAndFileEntryInfomation1(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();

																					displayDirectoryAndFileEntryInfomation2(&(fileBrowserData.findEntry));
																					sprintf(g_glcdBuf ,"result:%d", resultBuffer);
																					putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																					nextSequence();
*/
char findDirEntryUsingName(fat32Info *diskInfo, clustorData *searchingSecterInClustor, directoryAndFileEntryInformation *dirEntryInfo, CLUSTOR_LOCATION locateClustor, char *targetName)//indicateFirstClustor
{
	if(setDirBasicInfomation(&(*dirEntryInfo).dirStructure, targetName, 0, 0))
	{
		return -1;
	}
	setDirLongNameEntryInfomation(dirEntryInfo);
	setTargetLocation(&((*dirEntryInfo).entryInfo.location), locateClustor, 0);

						// sendString("set basic directory");
						// sprintf(g_strBuf.dat, "extension Entry count:%d", (*dirEntryInfo).entryInfo.extensionNameEntryCount);
						// sendString(g_strBuf.dat);
						// sendStringOnly("TargetName:");
						// sendString(targetName);
						// sendDirectoryAndFileEntryInfomation1(&(fileBrowserData.findEntry));
						// sendDirectoryAndFileEntryInfomation2(&(fileBrowserData.findEntry));
						// sendDirectoryAndFileEntryInfomation3(&(fileBrowserData.findEntry));

	if( *((*dirEntryInfo).dirStructure.dirName.simple+DIR_SIMPLE_NAME_MAXIMUM_LENGTH-2) != '~')
	{
						// sendString("find using simple name.");
		return findDirEntryUsingSimpleName(diskInfo, searchingSecterInClustor, dirEntryInfo);
	}
	else
	{
						// sendString("find using long name.");
		return findDirEntryUsingLongName(diskInfo, searchingSecterInClustor, dirEntryInfo);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char abstractTargetFileDirectoryEntryUsingName(fat32Info *diskInfo, clustorData *searchingSecterInClustor, directoryAndFileEntryInformation *physicalDirLocationInfo)//indicateFirstClustor
{
	if( !((*physicalDirLocationInfo).entryInfo.location.secterInClustor < ((*diskInfo).secterPerClustor)) )
	{
		(*physicalDirLocationInfo).entryInfo.location.secterInClustor=0;
	}

	char stringTemp[LONG_NAME_MAXIMUM_LENGTH]={0};
	char *str;
	unsigned char i;


	(*searchingSecterInClustor).locatedClustor=(*physicalDirLocationInfo).entryInfo.location.clustor;
	(*searchingSecterInClustor).secterInClustor=0;

	str = (*searchingSecterInClustor).secterData.data;


	//reset long name entry info
	(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount=0;

	checkFatAndLocatNextClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor);//if want check wrong fat table, added exception process

	do
	{
		if( !((*searchingSecterInClustor).secterInClustor < (*diskInfo).secterPerClustor) )//finding secter is lastest secter of clustor?
		{//lastest secter of clustor. loading next clustor
			(*searchingSecterInClustor).locatedClustor=(*searchingSecterInClustor).nextClustor;
			checkFatAndLocatNextClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor);
			(*searchingSecterInClustor).secterInClustor=0;
		}


		readSecterInClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor, (*searchingSecterInClustor).secterInClustor);

		for(str=(*searchingSecterInClustor).secterData.data; str<(*searchingSecterInClustor).secterData.data+SD_DATA_BUFFER_SIZE; str+=DIR_DISCRIPTION_LENGTH)
		{
			if((*str==DIR_DELEDTED)||(*str==DIR_EMPTY))////empty and delete
			{
				(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount=0;
				continue;
			}
			else if( ( (*(str+DIR_ATTR_OFFSET)) & ATTRIBUTE_MASK ) == ATTRIBUTE_MASK )//encountered long name entry.
			{
				if( ( (*(str)) & LONG_NAME_LASTEST_MASK ) == LONG_NAME_LASTEST_VALID_VALUE )//save first long name entry location info.
				{
					(*physicalDirLocationInfo).entryInfo.longNameLocation.clustor=(*searchingSecterInClustor).locatedClustor;
					(*physicalDirLocationInfo).entryInfo.longNameLocation.secterInClustor=(*searchingSecterInClustor).secterInClustor;
					(*physicalDirLocationInfo).entryInfo.longNameEntryOffset=str-(*searchingSecterInClustor).secterData.data;

					(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount=(*(str)&LONG_NAME_NUMBER_MASK);
				}
				abstractDirLongNameFromDirectoryEntry(str, stringTemp);
				continue;
			}//


			strncpy((*physicalDirLocationInfo).dirStructure.dirName.simple, str, DIR_SIMPLE_NAME_MAXIMUM_LENGTH);//general name copy
			strncpy((*physicalDirLocationInfo).dirStructure.dirName.extension, str+EXTENSION_OFFSET , DIR_EXTENSION_MAXUMUM_LENGTH);//general name copy

			dirDateAndTimeInfoParseFromDirectoryEntry(str, &((*physicalDirLocationInfo).dirStructure));

			if( *(str+DIR_LONG_NAME_MARKER_OFFSET) != DIR_LONG_NAME_MARKER )//fullName not exist;
			{
				for(i=0; i<(sizeof((*physicalDirLocationInfo).dirStructure.dirName.simple)/sizeof(char)); i++)
				{
					if(*(str+i)==0x20)
					{
						break;
					}
				}

				strncpy(stringTemp, (*physicalDirLocationInfo).dirStructure.dirName.simple, i);

				if( ( (*physicalDirLocationInfo).dirStructure.otherInfo.attribute = *(str+DIR_ATTR_OFFSET) ) != ATTR_DIRECTORY)//check '~'....
				{
					*(stringTemp+i) = '.';
					strncpy(stringTemp+i+1, (*physicalDirLocationInfo).dirStructure.dirName.extension, 3);
					*(stringTemp+i+1+3) = 0x00;
				}
				else
				{
					*(stringTemp+i) = 0x00;
				}
				dirShortNameChangeCapitalToSmalll(stringTemp, strlen(stringTemp));
			}

			/*encount valid entry*/
			if(strcmp((*physicalDirLocationInfo).dirStructure.dirName.fullName, stringTemp)==0)
			{

				/*!!!!if varing lastest access time, added function that abstract lastest access time from dir entry, this location...*/
				dirOtherInfoAbstractFromDirectoryEntry(str, &((*physicalDirLocationInfo).dirStructure.otherInfo));

				(*physicalDirLocationInfo).entryInfo.entryNumberOrOffset=str-(*searchingSecterInClustor).secterData.data;

				(*physicalDirLocationInfo).entryInfo.location.clustor=(*searchingSecterInClustor).locatedClustor;
				(*physicalDirLocationInfo).entryInfo.location.secterInClustor=(*searchingSecterInClustor).secterInClustor;

				return 0;
			}
			else
			{
				memset(stringTemp, 0x00, sizeof(stringTemp));
			}
				//reset long name entry info
				(*physicalDirLocationInfo).entryInfo.extensionNameEntryCount=0;

				(*physicalDirLocationInfo).entryInfo.longNameLocation.clustor=0;
				(*physicalDirLocationInfo).entryInfo.longNameLocation.secterInClustor=-1;
				(*physicalDirLocationInfo).entryInfo.longNameEntryOffset=-1;
		}
		(*physicalDirLocationInfo).entryInfo.location.secterInClustor++;

	}
	while( ((*searchingSecterInClustor).nextClustor != CLUSTOR_IS_END) || ((*searchingSecterInClustor).secterInClustor<((*diskInfo).secterPerClustor)) );

	(*physicalDirLocationInfo).entryInfo.location.secterInClustor=(*diskInfo).secterPerClustor-1;


	(*physicalDirLocationInfo).dirStructure.otherInfo.indicateFirstClustor = -1;
	(*physicalDirLocationInfo).dirStructure.otherInfo.indicateFirstClustor |= (-1<<16);

	(*physicalDirLocationInfo).entryInfo.entryNumberOrOffset=-1;

	(*physicalDirLocationInfo).entryInfo.location.clustor=-1;
	(*physicalDirLocationInfo).entryInfo.location.secterInClustor=-1;

	(*physicalDirLocationInfo).dirStructure.otherInfo.fileSize=-1;

	(*physicalDirLocationInfo).dirStructure.otherInfo.attribute=-1;

	return -1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char dirInfoConvertToDirectoryEntry(directoryStructure *p, char *dirEntry)
{
	strncpy(dirEntry+SIMPLE_NAME_OFFSET, (*p).dirName.simple, DIR_SIMPLE_NAME_MAXIMUM_LENGTH);
	strncpy(dirEntry+EXTENSION_OFFSET, (*p).dirName.extension, DIR_EXTENSION_MAXUMUM_LENGTH);

	convertZeroToSpace(dirEntry, DIR_SIMPLE_NAME_MAXIMUM_LENGTH+DIR_EXTENSION_MAXUMUM_LENGTH);

	*(dirEntry+DIR_ATTR_OFFSET) = (*p).otherInfo.attribute;


	parsing16BitsToLittleEndian(dirEntry+MSB_FIRST_CLUSTOR_OFFSET, ((*p).otherInfo.indicateFirstClustor>>16));
	parsing16BitsToLittleEndian(dirEntry+LSB_FIRST_CLUSTOR_OFFSET, (*p).otherInfo.indicateFirstClustor);

	/* copy file size to dir entry. */
						// sendString("/*make test print*/");
						// sprintf(g_strBuf.dat, "input file size is:0x%lx", (*p).otherInfo.fileSize);
						// sendString(g_strBuf.dat);

	parsing32BitsToLittleEndian(dirEntry+DIR_FILESIZE, (*p).otherInfo.fileSize);

	/*Input Dummy Time*/
	/*Create new dir entry so, creation, write and last access date are same*/
	/*Input dummy Date end*/
	// parsing16BitsToLittleEndian(dirEntry+DIR_CREATION_DATE_OFFSET, 0x3C21);
	// parsing16BitsToLittleEndian(dirEntry+DIR_WRITE_DATE_OFFSET, 0x3C21);
	// parsing16BitsToLittleEndian(dirEntry+DIR_LAST_ACCESS_DATE_OFFSET, 0x3C21);
	// parsing16BitsToLittleEndian(dirEntry+DIR_CREATION_DATE_OFFSET, dirDateInfoTransformTo16Bits(&((*p).dirStructure.createDataInfo.date)));
	// parsing16BitsToLittleEndian(dirEntry+DIR_WRITE_DATE_OFFSET, dirDateInfoTransformTo16Bits(&((*p).dirStructure.writeDateInfo.date)));
	// parsing16BitsToLittleEndian(dirEntry+DIR_LAST_ACCESS_DATE_OFFSET, dirDateInfoTransformTo16Bits(&((*p).dirStructure.lastestAccessDate.date)));
	// dateInfoConvertToDirectoryEntry(dirEntry, DIR_CREATION_DATE_OFFSET, &((*p).createDateInfo.date));
	// dateInfoConvertToDirectoryEntry(dirEntry, DIR_WRITE_DATE_OFFSET, &((*p).writeDateInfo.date));
	// dateInfoConvertToDirectoryEntry(dirEntry, DIR_LAST_ACCESS_DATE_OFFSET, &((*p).lastestAccessDate));

	/*Input Time*/
	// parsing16BitsToLittleEndian(dirEntry+DIR_CREATION_TIME_OFFSET, dirTimeInfoTransformTo16Bits(&((*p).dirStructure.createDataInfo.time)));
	// parsing16BitsToLittleEndian(dirEntry+DIR_WRITE_TIME_OFFSET, dirTimeInfoTransformTo16Bits(&((*p).dirStructure.writeDateInfo.time)));
	// timeInfoConvertToDirectoryEntry(dirEntry, DIR_CREATION_TIME_OFFSET, &((*p).createDateInfo.time));
	// timeInfoConvertToDirectoryEntry(dirEntry, DIR_WRITE_TIME_OFFSET, &((*p).writeDateInfo.time));
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void copyLongDirNameToBlockOfLongNameEntry(unsigned char i, unsigned char limit, unsigned char cmpOffsetCalibe, char *dirEntry, char **longName)
{
	if( (*(*longName)) != 0)
	{
		while(i<limit)
		{
			if( (*(*longName)) != 0)
			{
				*(dirEntry+i) = (*(*longName));
			}
			else
			{
				*(dirEntry+i) = (*(*longName));
				i+=LONG_DIR_NAME_ONE_WORD_SIZE;
				break;
			}
			(*longName)++;
			i+=LONG_DIR_NAME_ONE_WORD_SIZE;
		}
	}

	while(i<limit)
	{
		*(dirEntry+i) = 0xFF;
		*(dirEntry+i+1) = 0xFF;

		i+=LONG_DIR_NAME_ONE_WORD_SIZE;
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void copyFullNameToLongNameEntry(char *longNameEntry, char *longName)
{
	unsigned char cmpOffsetCalibe = LONG_DIR_NAME1_OFFSET;

	copyLongDirNameToBlockOfLongNameEntry(LONG_DIR_NAME1_OFFSET, LONG_DIR_NAME1_OFFSET+LONG_DIR_NAME1_SIZE, cmpOffsetCalibe, longNameEntry, &longName);
	cmpOffsetCalibe += LONG_DIR_NAME2_OFFSET-(LONG_DIR_NAME1_OFFSET+LONG_DIR_NAME1_SIZE);

	copyLongDirNameToBlockOfLongNameEntry(LONG_DIR_NAME2_OFFSET, LONG_DIR_NAME2_OFFSET+LONG_DIR_NAME2_SIZE, cmpOffsetCalibe, longNameEntry, &longName);
	cmpOffsetCalibe += LONG_DIR_NAME3_OFFSET-(LONG_DIR_NAME2_OFFSET+LONG_DIR_NAME2_SIZE);

	copyLongDirNameToBlockOfLongNameEntry(LONG_DIR_NAME3_OFFSET, LONG_DIR_NAME3_OFFSET+LONG_DIR_NAME3_SIZE, cmpOffsetCalibe, longNameEntry, &longName);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
