//#define F_CPU 16000000UL//CPU clock definition
#ifndef F_CPU
	#define F_CPU 16000000UL//CPU clock definition
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <util/twi.h>
#include <math.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* to save and display collected data, definition. */
#define STRING_BUFFER_SIZE 61
struct stringBuffer
{
	char dat[STRING_BUFFER_SIZE];
	char *loc;
	char dir;
}typedef stringBuffer;

stringBuffer g_strBuf;

char g_glcdBuf[STRING_BUFFER_SIZE];
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char SWITCH=0;
unsigned char testFlag = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define TEST_LED_DDR	DDRC
//#define TEST_LED		PORTC
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
	global variables
*/
//char stringBuffer[130] = {0};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void testDirStructurePrint(directoryStructure *p)
{
	sprintf(g_glcdBuf, "%s", (*p).dirName.fullName);
	putStringInGlcdAtPage(PAGE1, g_glcdBuf);

	sprintf(g_glcdBuf, "%s %s", (*p).dirName.simple, (*p).dirName.extension);
	putStringInGlcdAtPage(PAGE2, g_glcdBuf);

	sprintf(g_glcdBuf, "Attr 0x%02x", (*p).otherInfo.attribute);
	putStringInGlcdAtPage(PAGE3, g_glcdBuf);

	sprintf(g_glcdBuf, "Indicate First Clustor");
	putStringInGlcdAtPage(PAGE4, g_glcdBuf);

	sprintf(g_glcdBuf, "0x%000000008lx", (*p).otherInfo.indicateFirstClustor);
	putStringInGlcdAtPage(PAGE5, g_glcdBuf);

	sprintf(g_glcdBuf, "Size %ld", (*p).otherInfo.fileSize);
	putStringInGlcdAtPage(PAGE6, g_glcdBuf);
	putStringInGlcdAtPage(PAGE7, "                       ");
	putStringInGlcdAtPage(PAGE8, "                       ");
																					nextSequence();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
char findOffsetOfEndOfLongDirName(unsigned char i, unsigned char limit, unsigned char cmpOffsetCalibe, char *dirEntry)
{
	while(i<limit)
	{
		if( (*(dirEntry+i) == 0xFF) && (*(dirEntry+i+1) == 0xFF) )
		{
			return i;
		}

		i+=LONG_DIR_NAME_ONE_WORD_SIZE;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char findOffsetOfEndOfLongNameEntry(char *longNameEntry)
{
	unsigned char cmpOffsetCalibe = LONG_DIR_NAME1_OFFSET;
	char result;
	if(	(result = findOffsetOfEndOfLongDirName(LONG_DIR_NAME1_OFFSET, LONG_DIR_NAME1_OFFSET+LONG_DIR_NAME1_SIZE, cmpOffsetCalibe, longNameEntry)) ) return result;
	cmpOffsetCalibe += LONG_DIR_NAME2_OFFSET-(LONG_DIR_NAME1_OFFSET+LONG_DIR_NAME1_SIZE);
	
	if(	(result = findOffsetOfEndOfLongDirName(LONG_DIR_NAME2_OFFSET, LONG_DIR_NAME2_OFFSET+LONG_DIR_NAME2_SIZE, cmpOffsetCalibe, longNameEntry)) ) return result;
	cmpOffsetCalibe += LONG_DIR_NAME3_OFFSET-(LONG_DIR_NAME2_OFFSET+LONG_DIR_NAME2_SIZE);

	if(	(result = findOffsetOfEndOfLongDirName(LONG_DIR_NAME3_OFFSET, LONG_DIR_NAME3_OFFSET+LONG_DIR_NAME3_SIZE, cmpOffsetCalibe, longNameEntry)) ) return result;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void longNameConvertToOneDirectoryEntry(directoryAndFileEntryInformation *p, unsigned char longNameEntryNumber, char *dirEntry)
{
	memset(dirEntry, 0x00, DIR_DISCRIPTION_LENGTH);
	char *str = (*p).dirStructure.dirName.fullName+((longNameEntryNumber-1)*LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY);//fullName pointer moved
	
	unsigned char lastFlag=0;
	//long Directroy Number order
	if(longNameEntryNumber == (*p).entryInfo.extensionNameEntryCount)
	{
		longNameEntryNumber|=LONG_NAME_LASTEST_VALID_VALUE;
	}
	*(dirEntry)=longNameEntryNumber;

	*(dirEntry+DIR_ATTR_OFFSET)=ATTR_LONG_NAME;

	*(dirEntry+LONG_DIR_CHECK_SUM_OFFSET)=(*p).entryInfo.extensionNameChkSum;

	copyFullNameToLongNameEntry(dirEntry, str);

	
	//long Directroy Number order
	if(longNameEntryNumber == (*p).entryInfo.extensionNameEntryCount)
	{
		longNameEntryNumber|=LONG_NAME_LASTEST_VALID_VALUE;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char writeDirInfoToDirectoryEntry(fat32Info *diskInfo, clustorData *searchingSecterInClustor, directoryAndFileEntryInformation *p)
{
	if( !((*searchingSecterInClustor).secterInClustor < ((*diskInfo).secterPerClustor)) )
	{
		return -1;
	}

	char *str;
	unsigned char longNameEntryNumber = (*p).entryInfo.extensionNameEntryCount;
	unsigned int entryOffsetBuffer=(*p).entryInfo.entryNumberOrOffset;
	
	(*searchingSecterInClustor).locatedClustor=(*p).entryInfo.location.clustor;
	(*searchingSecterInClustor).secterInClustor=(*p).entryInfo.location.secterInClustor;

								// sprintf(g_strBuf.dat, "L.N.E.C:%d ", (*p).entryInfo.extensionNameEntryCount);
								// sendString(g_strBuf.dat);
					
	checkFatAndLocatNextClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor);//if want check wrong fat table, added exception process

	do
	{
		if( !((*searchingSecterInClustor).secterInClustor < (*diskInfo).secterPerClustor) )//finding secter is lastest secter of clustor?
		{//lastest secter of clustor. loading next clustor
			//writeSecterInClustor(diskInfo, searchingSecterInClustor, (*p).entryInfo.location.clustor, (*p).entryInfo.location.secterInClustor-1);
			(*searchingSecterInClustor).locatedClustor=(*searchingSecterInClustor).nextClustor;
			checkFatAndLocatNextClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor);
			(*searchingSecterInClustor).secterInClustor=0;
		}		


		readSecterInClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor, (*searchingSecterInClustor).secterInClustor);


		for(str=(*searchingSecterInClustor).secterData.data+entryOffsetBuffer; str<(*searchingSecterInClustor).secterData.data+SD_DATA_BUFFER_SIZE; str+=DIR_DISCRIPTION_LENGTH)
		{		
			if(longNameEntryNumber!=0)
			{
				longNameConvertToOneDirectoryEntry(p, longNameEntryNumber, str);
				longNameEntryNumber--;
			}
			else
			{
				/*  */
				dirInfoConvertToDirectoryEntry(&((*p).dirStructure), str);
				/*  */
				dateInfoConvertToDirectoryEntry(str, DIR_CREATION_DATE_OFFSET, &((*p).dirStructure.createDateInfo.date));
				dateInfoConvertToDirectoryEntry(str, DIR_LAST_ACCESS_DATE_OFFSET, &((*p).dirStructure.writeDateInfo.date));
				dateInfoConvertToDirectoryEntry(str, DIR_WRITE_DATE_OFFSET, &((*p).dirStructure.lastestAccessDate));

				timeInfoConvertToDirectoryEntry(str, DIR_CREATION_TIME_OFFSET, &((*p).dirStructure.createDateInfo.time));
				timeInfoConvertToDirectoryEntry(str, DIR_WRITE_TIME_OFFSET, &((*p).dirStructure.writeDateInfo.time));
	
				writeSecterInClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor, (*searchingSecterInClustor).secterInClustor);
				return 0;
			}
		}
		writeSecterInClustor(diskInfo, searchingSecterInClustor, (*searchingSecterInClustor).locatedClustor, (*searchingSecterInClustor).secterInClustor);
		(*searchingSecterInClustor).secterInClustor++;
		entryOffsetBuffer=0;
	}
	while( ((*searchingSecterInClustor).nextClustor != CLUSTOR_IS_END) || ((*searchingSecterInClustor).secterInClustor<((*diskInfo).secterPerClustor)) );

	//if target is not found, (*p).entryInfo.location.clustor is indicate lastest clustor, that is found by this function.
	return -1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*folder creating....*//*folder creating....*//*folder creating....*//*folder creating....*//*folder creating....*/
/*test Codes*/
/*
	resultBuffer=254;
	resultBuffer=createNewDirEntry(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), (sdCardInfo).rootClustor, ATTR_DIRECTORY, "kword56t");

												if( strlen(fileBrowserData.findEntry.dirStructure.dirName.fullName) != 0 )
												{
													sprintf(g_glcdBuf ,"%s", fileBrowserData.findEntry.dirStructure.dirName.fullName);
												}
												else
												{
													sprintf(g_glcdBuf ,"%s.%s", fileBrowserData.findEntry.dirStructure.dirName.simple, fileBrowserData.findEntry.dirStructure.dirName.extension);
												}
												putStringInGlcdAtPage(PAGE1, g_glcdBuf);
												
												if(fileBrowserData.findEntry.dirStructure.otherInfo.attribute == ATTR_DIRECTORY)
												{
													sprintf(g_glcdBuf ,"Directory");		
												}
												else if(fileBrowserData.findEntry.dirStructure.otherInfo.attribute == ATTR_ARCHIVE)
												{
													sprintf(g_glcdBuf ,"Archive");		
												}
												else
												{
													sprintf(g_glcdBuf ,"Attribute:0x%x", fileBrowserData.findEntry.dirStructure.otherInfo.attribute);
												}
												putStringInGlcdAtPage(PAGE2, g_glcdBuf);

												
												sprintf(g_glcdBuf ,"location 0x%lx", fileBrowserData.findEntry.entryInfo.location.clustor);
												putStringInGlcdAtPage(PAGE3, g_glcdBuf);
												
												sprintf(g_glcdBuf ,"indicate 0x%lx", fileBrowserData.findEntry.dirStructure.otherInfo.indicateFirstClustor);
												putStringInGlcdAtPage(PAGE4, g_glcdBuf);
												
												
												sprintf(g_glcdBuf ,"%d", resultBuffer);
												putStringInGlcdAtPage(PAGE8, g_glcdBuf);
												nextSequence();

																						
																						

	resultBuffer=254;
	resultBuffer=createNewDirEntry(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), (sdCardInfo).rootClustor, ATTR_DIRECTORY, "qpoelf3oe2pkf.txt");

																						if( strlen(fileBrowserData.findEntry.dirStructure.dirName.fullName) != 0 )
												{
													sprintf(g_glcdBuf ,"%s", fileBrowserData.findEntry.dirStructure.dirName.fullName);
												}
												else
												{
													sprintf(g_glcdBuf ,"%s.%s", fileBrowserData.findEntry.dirStructure.dirName.simple, fileBrowserData.findEntry.dirStructure.dirName.extension);
												}
												putStringInGlcdAtPage(PAGE1, g_glcdBuf);
												
												if(fileBrowserData.findEntry.dirStructure.otherInfo.attribute == ATTR_DIRECTORY)
												{
													sprintf(g_glcdBuf ,"Directory");		
												}
												else if(fileBrowserData.findEntry.dirStructure.otherInfo.attribute == ATTR_ARCHIVE)
												{
													sprintf(g_glcdBuf ,"Archive");		
												}
												else
												{
													sprintf(g_glcdBuf ,"Attribute:0x%x", fileBrowserData.findEntry.dirStructure.otherInfo.attribute);
												}
												putStringInGlcdAtPage(PAGE2, g_glcdBuf);

												
												sprintf(g_glcdBuf ,"location 0x%lx", fileBrowserData.findEntry.entryInfo.location.clustor);
												putStringInGlcdAtPage(PAGE3, g_glcdBuf);
												
												sprintf(g_glcdBuf ,"indicate 0x%lx", fileBrowserData.findEntry.dirStructure.otherInfo.indicateFirstClustor);
												putStringInGlcdAtPage(PAGE4, g_glcdBuf);
												
												
												sprintf(g_glcdBuf ,"%d", resultBuffer);
												putStringInGlcdAtPage(PAGE8, g_glcdBuf);
												nextSequence();

	resultBuffer=254;
	resultBuffer=createNewDirEntry(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), (sdCardInfo).rootClustor, ATTR_DIRECTORY, "mvkd6rt9ef1g5fgw6w5t1h.txt");

												if( strlen(fileBrowserData.findEntry.dirStructure.dirName.fullName) != 0 )
												{
													sprintf(g_glcdBuf ,"%s", fileBrowserData.findEntry.dirStructure.dirName.fullName);
												}
												else
												{
													sprintf(g_glcdBuf ,"%s.%s", fileBrowserData.findEntry.dirStructure.dirName.simple, fileBrowserData.findEntry.dirStructure.dirName.extension);
												}
												putStringInGlcdAtPage(PAGE1, g_glcdBuf);
												
												if(fileBrowserData.findEntry.dirStructure.otherInfo.attribute == ATTR_DIRECTORY)
												{
													sprintf(g_glcdBuf ,"Directory");		
												}
												else if(fileBrowserData.findEntry.dirStructure.otherInfo.attribute == ATTR_ARCHIVE)
												{
													sprintf(g_glcdBuf ,"Archive");		
												}
												else
												{
													sprintf(g_glcdBuf ,"Attribute:0x%x", fileBrowserData.findEntry.dirStructure.otherInfo.attribute);
												}
												putStringInGlcdAtPage(PAGE2, g_glcdBuf);

												
												sprintf(g_glcdBuf ,"location 0x%lx", fileBrowserData.findEntry.entryInfo.location.clustor);
												putStringInGlcdAtPage(PAGE3, g_glcdBuf);
												
												sprintf(g_glcdBuf ,"indicate 0x%lx", fileBrowserData.findEntry.dirStructure.otherInfo.indicateFirstClustor);
												putStringInGlcdAtPage(PAGE4, g_glcdBuf);
												
												
												sprintf(g_glcdBuf ,"%d", resultBuffer);
												putStringInGlcdAtPage(PAGE8, g_glcdBuf);
												nextSequence();
*/
char createNewDirEntry(fat32Info *diskInfo, clustorData *bufferSecterInClustor, directoryAndFileEntryInformation *p, CLUSTOR_LOCATION firstEntryClustor,  unsigned char attribute, char *fileName)
{
/*	//using example code
	directoryAndFileEntryInformation *findDirEntry = (directoryAndFileEntryInformation*)malloc(sizeof(directoryAndFileEntryInformation));
	char resultBuffer;
	resultBuffer=createNewDirEntry(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), (sdCardInfo).rootClustor, ATTR_DIRECTORY, "atest001");
*/
	//if want to make dir, have to set empty clustor!...
	if(strlen(fileName)==0)
	{
		return 1;
	}
	if( ((attribute&ATTR_DIR_FILE_MASK)!=ATTR_ARCHIVE) && ((attribute&ATTR_DIR_FILE_MASK)!=ATTR_DIRECTORY) )
	{
		return 2;
	}	
	
	if(setDirBasicInfomation(&((*p).dirStructure), fileName, attribute, 0))
	{
		return 3;
	}
	setDirLongNameEntryInfomation(p);
	//set long name info and location info start//
	setTargetLocation(&((*p).entryInfo.location), firstEntryClustor, 0);
	//set long name info and location info end//
									// sprintf(g_strBuf.dat, "In create af set basic:%d", (*p).entryInfo.extensionNameEntryCount);
									// sendString(g_strBuf.dat);
	/*Same name find section start*/
	// if(strlen((*p).dirStructure.dirName.fullName)==0)
	if(*((*p).dirStructure.dirName.simple+DIR_SIMPLE_NAME_MAXIMUM_LENGTH-2) != '~')
	{
									// sendString("notExistSameSimpleName");
									// sprintf(g_strBuf.dat, "entryInfo.extensionNameEntryCount:%d", (*p).entryInfo.extensionNameEntryCount);
									// sendString(g_strBuf.dat);
		if(notExistSameSimpleName(diskInfo, bufferSecterInClustor, (*p).entryInfo.location.clustor, &((*p).dirStructure.dirName)) != 0)
		{
			return 4;
		}
	}
	else
	{
									// sendString("notExistSameLongName");
									// sprintf(g_strBuf.dat, "entryInfo.extensionNameEntryCount:%d", (*p).entryInfo.extensionNameEntryCount);
									// sendString(g_strBuf.dat);
		if(notExistSameLongName(diskInfo, bufferSecterInClustor, (*p).entryInfo.location.clustor, &((*p).dirStructure.dirName)) != 0)
		{
			return 5;
		}

		while(notExistSameSimpleName(diskInfo, bufferSecterInClustor, (*p).entryInfo.location.clustor, &((*p).dirStructure.dirName)) != 0)//This loop can occur infinite loop.
		{
			createSimpleName( (*p).dirStructure.dirName.simple );
			setTargetLocation(&((*p).entryInfo.location), firstEntryClustor, 0);
		}
		(*p).entryInfo.extensionNameChkSum = generateLongNameCheckSum((*p).dirStructure.dirName.simple, (*p).dirStructure.dirName.extension);
									// sprintf(g_strBuf.dat, "In create af notExiSimplName2:%d", (*p).entryInfo.extensionNameEntryCount);
									// sendString(g_strBuf.dat);
	}
	//Same name find section end//

	//find empty dir entry and write dir entry start//
	//find empry dir entry. if findEmptyDirEntry found empty entry, return target location, that is wrote at entryInfo. But cannot found, it return latest access clustor.
	if(findEmptyDirEntry(diskInfo, bufferSecterInClustor, firstEntryClustor, (*p).entryInfo.extensionNameEntryCount+1, &((*p).entryInfo)))//return -1, so enough empty directory entry not exist.
	{
		setTargetLocation(&((*p).entryInfo.location), writeNextClustor(diskInfo, bufferSecterInClustor, (*p).entryInfo.location.clustor, findEmptyClustor(diskInfo, bufferSecterInClustor, (*p).entryInfo.location.clustor)), 0);
		(*p).entryInfo.entryNumberOrOffset=0;
	}
									// sprintf(g_strBuf.dat, "In create af findEmptyDirEntr:%d", (*p).entryInfo.extensionNameEntryCount);
									// sendString(g_strBuf.dat);
	//find empty dir entry and write dir entry end//
	
	//update date & time start
	updateDateFromDS1302(&((*p).dirStructure.createDateInfo.date));
	memcpy(&((*p).dirStructure.writeDateInfo.date), &((*p).dirStructure.createDateInfo.date), sizeof(dirDateInfo));
	memcpy(&((*p).dirStructure.lastestAccessDate), &((*p).dirStructure.createDateInfo.date), sizeof(dirDateInfo));

	updateTimeFromDS1302(&((*p).dirStructure.createDateInfo.time));
	memcpy(&((*p).dirStructure.writeDateInfo.time), &((*p).dirStructure.createDateInfo.time), sizeof(dirTimeInfo));
	//update date & time end
	
	//Because directory have indicate clustor, so empty clustor must is allocated. check valid clustor address start//
	if(((*p).dirStructure.otherInfo.attribute&ATTR_DIR_FILE_MASK) == ATTR_DIRECTORY)//if make dir entry is directory, must make entry directory clustor.
	{
		unsigned char i;

		//if create entry is directory, find empry clustor and write next clustor. start//
		(*p).dirStructure.otherInfo.indicateFirstClustor=writeNewClustor(diskInfo, bufferSecterInClustor, findEmptyClustor(diskInfo, bufferSecterInClustor, (*p).entryInfo.location.clustor));
		memset((*bufferSecterInClustor).secterData.data, 0x00, SD_DATA_BUFFER_SIZE);
	
		for(i=0; i<DIR_SIMPLE_NAME_MAXIMUM_LENGTH+DIR_EXTENSION_MAXUMUM_LENGTH; i++)//"." dir name blank set.
		{
			*((*bufferSecterInClustor).secterData.data+i) = DIR_NAME_EMPTY_DATA;
		}
		*((*bufferSecterInClustor).secterData.data) = '.';
		parsing16BitsToLittleEndian(((*bufferSecterInClustor).secterData.data+MSB_FIRST_CLUSTOR_OFFSET), ((*p).dirStructure.otherInfo.indicateFirstClustor>>16));
		parsing16BitsToLittleEndian(((*bufferSecterInClustor).secterData.data+LSB_FIRST_CLUSTOR_OFFSET), (*p).dirStructure.otherInfo.indicateFirstClustor);
		
		*((*bufferSecterInClustor).secterData.data+DIR_ATTR_OFFSET) = ATTR_DIRECTORY;
		
		/*Input dummy Date start*/
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_CREATION_DATE_OFFSET, 0x3C21);
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_WRITE_DATE_OFFSET, 0x3C21);
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_LAST_ACCESS_DATE_OFFSET, 0x3C21);
		/*Input dummy Date end*/
		/*Create new dir entry so, creation, write and last access date are same*/
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_CREATION_DATE_OFFSET, dirDateInfoTransformTo16Bits&((*p).dirStructure.createDataInfo.date)));
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_WRITE_DATE_OFFSET, dirDateInfoTransformTo16Bits&((*p).dirStructure.createDataInfo.date)));
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_LAST_ACCESS_DATE_OFFSET, dirDateInfoTransformTo16Bits&((*p).dirStructure.createDataInfo.date)));
		dateInfoConvertToDirectoryEntry((*bufferSecterInClustor).secterData.data, DIR_CREATION_DATE_OFFSET, &((*p).dirStructure.createDateInfo.date));
		dateInfoConvertToDirectoryEntry((*bufferSecterInClustor).secterData.data, DIR_WRITE_DATE_OFFSET, &((*p).dirStructure.writeDateInfo.date));
		dateInfoConvertToDirectoryEntry((*bufferSecterInClustor).secterData.data, DIR_LAST_ACCESS_DATE_OFFSET, &((*p).dirStructure.lastestAccessDate));
	
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+
		timeInfoConvertToDirectoryEntry((*bufferSecterInClustor).secterData.data, DIR_CREATION_TIME_OFFSET, &((*p).dirStructure.createDateInfo.time));
		timeInfoConvertToDirectoryEntry((*bufferSecterInClustor).secterData.data, DIR_WRITE_TIME_OFFSET, &((*p).dirStructure.writeDateInfo.time));

		
		for(i=DIR_DISCRIPTION_LENGTH; i<DIR_SIMPLE_NAME_MAXIMUM_LENGTH+DIR_EXTENSION_MAXUMUM_LENGTH+DIR_DISCRIPTION_LENGTH; i++)//"." dir name blank set.
		{
			*((*bufferSecterInClustor).secterData.data+i) = DIR_NAME_EMPTY_DATA;
		}
		*((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH) = '.';
		*((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH+1) = '.';
		if(firstEntryClustor != (*diskInfo).rootClustor)
		{
			parsing16BitsToLittleEndian(((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH+MSB_FIRST_CLUSTOR_OFFSET), (firstEntryClustor>>16));
			parsing16BitsToLittleEndian(((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH+LSB_FIRST_CLUSTOR_OFFSET), firstEntryClustor);
		}
		
		*((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH+DIR_ATTR_OFFSET) = ATTR_DIRECTORY;

		/*Input dummy Date start*/
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_DISCRIPdTION_LENGTH+DIR_CREATION_DATE_OFFSET, 0x3C21);
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH+DIR_WRITE_DATE_OFFSET, 0x3C21);
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH+DIR_LAST_ACCESS_DATE_OFFSET, 0x3C21);
		/*Input dummy Date end*/
		/*Create new dir entry so, creation, write and last access date are same*/
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_DISCRIPdTION_LENGTH+DIR_CREATION_DATE_OFFSET, dirDateInfoTransformTo16Bits&((*p).dirStructure.createDataInfo.date)));
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH+DIR_WRITE_DATE_OFFSET, dirDateInfoTransformTo16Bits&((*p).dirStructure.createDataInfo.date)));
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH+DIR_LAST_ACCESS_DATE_OFFSET, dirDateInfoTransformTo16Bits&((*p).dirStructure.createDataInfo.date)));
		dateInfoConvertToDirectoryEntry((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH, DIR_CREATION_DATE_OFFSET, &((*p).dirStructure.createDateInfo.date));
		dateInfoConvertToDirectoryEntry((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH, DIR_WRITE_DATE_OFFSET, &((*p).dirStructure.createDateInfo.date));
		dateInfoConvertToDirectoryEntry((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH, DIR_LAST_ACCESS_DATE_OFFSET, &((*p).dirStructure.createDateInfo.date));

		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_DISCRIPdTION_LENGTH+
		// parsing16BitsToLittleEndian((*bufferSecterInClustor).secterData.data+DIR_DISCRIPdTION_LENGTH+
		timeInfoConvertToDirectoryEntry((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH, DIR_CREATION_TIME_OFFSET, &((*p).dirStructure.createDateInfo.time));
		timeInfoConvertToDirectoryEntry((*bufferSecterInClustor).secterData.data+DIR_DISCRIPTION_LENGTH, DIR_WRITE_TIME_OFFSET, &((*p).dirStructure.createDateInfo.time));

		
		writeSecterInClustor(diskInfo, bufferSecterInClustor, (*p).dirStructure.otherInfo.indicateFirstClustor, 0);
		//if create entry is directory, find empry clustor and write next clustor. end//
	}
	else if(((*p).dirStructure.otherInfo.attribute&ATTR_DIR_FILE_MASK) == ATTR_ARCHIVE)
	{
		/*
		File is make in first time, it no have any values of size and indicate location clustor.
		So, if file is made first time, must to reset values of file size and indicate clustor.
		*/
		(*p).dirStructure.otherInfo.fileSize=0;
		(*p).dirStructure.otherInfo.indicateFirstClustor=0;
	}
	//check valid clustor address end//
	
	//Part of writing new dirEntry
	writeDirInfoToDirectoryEntry(diskInfo, bufferSecterInClustor, p);
									// sprintf(g_strBuf.dat, "In create af writeDirInfoToDi:%d", (*p).entryInfo.extensionNameEntryCount);
									// sendString(g_strBuf.dat);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char findDirEntryIfNotCreateNewDirEntry(fat32Info *diskInfo, clustorData *bufferSecterInClustor, directoryAndFileEntryInformation *p, CLUSTOR_LOCATION targetClustor,  unsigned char attribute, char *fileName)
{
	unsigned char resultBuffer=0;
	/*using findTargetFileDirectoryEntryUsingName start*/
	// if((resultBuffer=findDirEntryUsingName(diskInfo, bufferSecterInClustor, p, targetClustor, fileName))!=0)
	// {
		// if((resultBuffer=createNewDirEntry(diskInfo, bufferSecterInClustor, p, targetClustor, attribute, fileName))==0)
		// {
			// resultBuffer=findDirEntryUsingName(diskInfo, bufferSecterInClustor, p, targetClustor, fileName);
		// }
	// }
	if((resultBuffer=findDirEntryUsingName(diskInfo, bufferSecterInClustor, p, targetClustor, fileName)))
	{
		if((resultBuffer=createNewDirEntry(diskInfo, bufferSecterInClustor, p, targetClustor, attribute, fileName)))
		{
			return -1;		
		}
		else
		{
			if((resultBuffer=findDirEntryUsingName(diskInfo, bufferSecterInClustor, p, targetClustor, fileName)))
			{
				return -2;
			}
		}
	}
	/*using findTargetFileDirectoryEntryUsingName end*/


	/*check indicate first clustor is zero, and size zero start*/
	if( (*p).dirStructure.otherInfo.indicateFirstClustor == 0 )
	{
		if( (*p).dirStructure.otherInfo.fileSize == 0 )//If file size is zero, set indicate clustor in directory entry and operate FAT.
		{
			(*p).dirStructure.otherInfo.indicateFirstClustor=findEmptyClustor(diskInfo, bufferSecterInClustor, (*p).entryInfo.location.clustor);

			writeNewClustor(diskInfo, bufferSecterInClustor, (*p).dirStructure.otherInfo.indicateFirstClustor);
			
			readSecterInClustor(diskInfo, bufferSecterInClustor, (*p).entryInfo.location.clustor, (*p).entryInfo.location.secterInClustor);
			parsing16BitsToLittleEndian( ((*bufferSecterInClustor).secterData.data+(*p).entryInfo.entryNumberOrOffset+MSB_FIRST_CLUSTOR_OFFSET), ((unsigned int)((*p).dirStructure.otherInfo.indicateFirstClustor>>16)) );//set indicate clustor bits MSB
			parsing16BitsToLittleEndian( ((*bufferSecterInClustor).secterData.data+(*p).entryInfo.entryNumberOrOffset+LSB_FIRST_CLUSTOR_OFFSET), ((unsigned int)((*p).dirStructure.otherInfo.indicateFirstClustor)) );//set indicate clustor bits LSB
			writeSecterInClustor(diskInfo, bufferSecterInClustor, (*p).entryInfo.location.clustor, (*p).entryInfo.location.secterInClustor);

			memset((*bufferSecterInClustor).secterData.data, 0x00, SD_DATA_BUFFER_SIZE);
			writeSecterInClustor(diskInfo, bufferSecterInClustor, (*p).dirStructure.otherInfo.indicateFirstClustor, 0);
		}

		resultBuffer=findDirEntryUsingName(diskInfo, bufferSecterInClustor, p, targetClustor, (*p).dirStructure.dirName.fullName);
	}
	/*check indicate first clustor is zero, and size zero end*/
	
	return resultBuffer;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char deleteDirEntryAtIndicateClustor(fat32Info *diskInfo, clustorData *bufferSecterInClustor, CLUSTOR_LOCATION parentEntryClustor, CLUSTOR_LOCATION locatedEntryClustor)
{
	/* not support dir entry have long name implementation */
	char *str;
	
	CLUSTOR_LOCATION deletionClustor=locatedEntryClustor;
	unsigned char deletionSecterInClustor=0;
	
	CLUSTOR_LOCATION entryIndicateLocation=0;
	

	checkFatAndLocatNextClustor(diskInfo, bufferSecterInClustor, deletionClustor);//if want check wrong fat table, added exception process

	do
	{
		if( !( deletionSecterInClustor < (*diskInfo).secterPerClustor) )//finding secter is lastest secter of clustor?
		{//lastest secter of clustor. loading next clustor
			deletionClustor=(*bufferSecterInClustor).nextClustor;
			checkFatAndLocatNextClustor(diskInfo, bufferSecterInClustor, deletionClustor);
			 deletionSecterInClustor=0;
		}		


		readSecterInClustor(diskInfo, bufferSecterInClustor, deletionClustor,  deletionSecterInClustor);

		str=(*bufferSecterInClustor).secterData.data;

		while(str<(*bufferSecterInClustor).secterData.data+SD_DATA_BUFFER_SIZE)
		{
			if((*str==DIR_DELEDTED)||(*str==DIR_EMPTY))//deleted or empty entry
			{
				str+=DIR_DISCRIPTION_LENGTH;
				continue;
			}
			else if( ( (*(str+DIR_ATTR_OFFSET)) & ATTRIBUTE_MASK ) == ATTRIBUTE_MASK )//encountered long name entry. When encount long name entry, delete with no condition.
			{
				str+=DIR_DISCRIPTION_LENGTH;
				continue;
			}

			//abstract file's indicate clustor address.
			entryIndicateLocation = abstractLittleEndianTo16Bits(str+LSB_FIRST_CLUSTOR_OFFSET);
			entryIndicateLocation |= (((unsigned long)abstractLittleEndianTo16Bits(str+MSB_FIRST_CLUSTOR_OFFSET))<<16);
			if(entryIndicateLocation == parentEntryClustor)
			{
				str+=DIR_DISCRIPTION_LENGTH;
				continue;
			}
			else if(entryIndicateLocation == locatedEntryClustor)
			{
				str+=DIR_DISCRIPTION_LENGTH;
				continue;
			}
			else if(entryIndicateLocation == 0)//file is not allocate clustor, folder is indicate root entry.
			{
				str+=DIR_DISCRIPTION_LENGTH;
				continue;
			}

			if((*(str+DIR_ATTR_OFFSET)&ATTR_DIR_FILE_MASK) == ATTR_ARCHIVE)
			{
				deleteClustor(diskInfo, bufferSecterInClustor, entryIndicateLocation);//variation file allocation table. already deleted clustor.
				checkFatAndLocatNextClustor(diskInfo, bufferSecterInClustor, deletionClustor);
				readSecterInClustor(diskInfo, bufferSecterInClustor, deletionClustor,  deletionSecterInClustor);
				//(*str)=DIR_DELEDTED;
			}
			else if((*(str+DIR_ATTR_OFFSET)&ATTR_DIR_FILE_MASK) == ATTR_DIRECTORY)//deletion of directory is needs check inner dir entry.
			{
				deleteDirEntryAtIndicateClustor(diskInfo, bufferSecterInClustor, locatedEntryClustor, entryIndicateLocation);

				checkFatAndLocatNextClustor(diskInfo, bufferSecterInClustor, deletionClustor);
				readSecterInClustor(diskInfo, bufferSecterInClustor, deletionClustor,  deletionSecterInClustor);
			}
			
			str+=DIR_DISCRIPTION_LENGTH;
		}
		
		deletionSecterInClustor++;
	}
	while( ((*bufferSecterInClustor).nextClustor != CLUSTOR_IS_END) || ( deletionSecterInClustor<((*diskInfo).secterPerClustor)) );

	deleteClustor(diskInfo, bufferSecterInClustor, locatedEntryClustor);
	
	// if target is not found, (*physicalDirLocationInfo).entryInfo.location.clustor is indicate lastest clustor, that is found by this function.
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char deleteDirEntry(fat32Info *diskInfo, clustorData *bufferSecterInClustor, CLUSTOR_LOCATION deleteEntryLocatedClustor, unsigned char deleteEntryLocatedSecterInClustor, ENTRY_OR_OFFSET deleteEntryLocatedEntryOffset)
{
	if(deleteEntryLocatedClustor < 2)
	{
		return 1;
	}
	else if(!(deleteEntryLocatedSecterInClustor<(*diskInfo).secterPerClustor))
	{
		return 2;
	}
	
	
	char *str;
	CLUSTOR_LOCATION entryIndicateLocation=0;
	
	
	checkFatAndLocatNextClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor);//if want check wrong fat table, added exception process

	do
	{
		if( !( deleteEntryLocatedSecterInClustor < (*diskInfo).secterPerClustor) )//finding secter is lastest secter of clustor?
		{//lastest secter of clustor. loading next clustor
			deleteEntryLocatedClustor=(*bufferSecterInClustor).nextClustor;
			checkFatAndLocatNextClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor);
			deleteEntryLocatedSecterInClustor=0;
		}		


		readSecterInClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor,  deleteEntryLocatedSecterInClustor);

		str=(*bufferSecterInClustor).secterData.data+deleteEntryLocatedEntryOffset;
		deleteEntryLocatedEntryOffset=0;

		while(str<(*bufferSecterInClustor).secterData.data+SD_DATA_BUFFER_SIZE)
		{
			if( ( (*(str+DIR_ATTR_OFFSET)) & ATTRIBUTE_MASK ) == ATTRIBUTE_MASK )//encountered long name entry. When encount long name entry, delete with no condition.
			{
				*(str)=DIR_DELEDTED;
				str+=DIR_DISCRIPTION_LENGTH;
				continue;
			}

			//abstract file's indicate clustor address.
			entryIndicateLocation = abstractLittleEndianTo16Bits(str+LSB_FIRST_CLUSTOR_OFFSET);
			entryIndicateLocation |= (((unsigned long)abstractLittleEndianTo16Bits(str+MSB_FIRST_CLUSTOR_OFFSET))<<16);
			if(entryIndicateLocation==0)
			{
				(*str)=DIR_DELEDTED;
				writeSecterInClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor, deleteEntryLocatedSecterInClustor);
				return 0;
			}
			else if(entryIndicateLocation==deleteEntryLocatedClustor)
			{
				return 0;
			}
			(*str)=DIR_DELEDTED;
			writeSecterInClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor, deleteEntryLocatedSecterInClustor);
			
			if((*(str+DIR_ATTR_OFFSET)&ATTR_DIR_FILE_MASK) == ATTR_ARCHIVE)
			{
				deleteClustor(diskInfo, bufferSecterInClustor, entryIndicateLocation);//variation file allocation table. already deleted clustor.
				
				// checkFatAndLocatNextClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor);
				// readSecterInClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor, deleteEntryLocatedSecterInClustor);
			}
			else if((*(str+DIR_ATTR_OFFSET)&ATTR_DIR_FILE_MASK) == ATTR_DIRECTORY)//deletion of directory is needs check inner dir entry.
			{
				deleteDirEntryAtIndicateClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor, entryIndicateLocation);
				
				// checkFatAndLocatNextClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor);
				// readSecterInClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor, deleteEntryLocatedSecterInClustor);
			}
			// (*str)=DIR_DELEDTED;
			// writeSecterInClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor, deleteEntryLocatedSecterInClustor);
			return 0;
		}
		writeSecterInClustor(diskInfo, bufferSecterInClustor, deleteEntryLocatedClustor, deleteEntryLocatedSecterInClustor);
		deleteEntryLocatedSecterInClustor++;
	}
	while( ((*bufferSecterInClustor).nextClustor != CLUSTOR_IS_END) || ( deleteEntryLocatedSecterInClustor<((*diskInfo).secterPerClustor)) );
	// if target is not found, (*physicalDirLocationInfo).entryInfo.location.clustor is indicate lastest clustor, that is found by this function.
	return -1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char findDirEntryAndDeleteUsingName(fat32Info *diskInfo, clustorData *bufferSecterInClustor, directoryAndFileEntryInformation *dirEntryInfo, CLUSTOR_LOCATION entryLocationClustor, char *targetName)
{
/*	//test codes
	findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), targetClustor, fileName);
	deleteDirEntry(&sdCardInfo, &clustor,(fileBrowserData.findEntry).entryInfo.location.clustor, (fileBrowserData.findEntry).entryInfo.location.secterInClustor, (fileBrowserData.findEntry).entryInfo.entryNumberOrOffset);
*/
/* //using example code
	CLUSTOR LOCATIONtargetClustor=sdCardInfo.rootClustor;
	strcpy(fileName, "123456789101112131415");

	findDirEntryAndDeleteUsingName(&sdCardInfo, &clustor, &fileBrowserData.findEntry, targetClustor, fileName);
*/

	char resultBuffer=0;
	if((resultBuffer=findDirEntryUsingName(diskInfo, bufferSecterInClustor, dirEntryInfo, entryLocationClustor, targetName))==0)
	{
		/*
			if targeted dir entry have extra long name entry, delete location(clustor, secter number in clustor and dir entry offset is upper than dir entry.
			And findDirEntryUsingName return also dir entry location and extra long name location. So directoryAndFileEntryInformation have both locations these are dir entry and extra dir entry.
			Also names saved these values are different.
		*/
		// if((*dirEntryInfo).entryInfo.extensionNameEntryCount)//long name check
		if( *((*dirEntryInfo).dirStructure.dirName.simple+DIR_SIMPLE_NAME_MAXIMUM_LENGTH-2) != '~')
		{/*Simple name*/
			resultBuffer=deleteDirEntry(diskInfo, bufferSecterInClustor, (*dirEntryInfo).entryInfo.location.clustor, (*dirEntryInfo).entryInfo.location.secterInClustor, (*dirEntryInfo).entryInfo.entryNumberOrOffset);
		}
		else
		{/*LongName*/
			resultBuffer=deleteDirEntry(diskInfo, bufferSecterInClustor, (*dirEntryInfo).entryInfo.longNameLocation.clustor, (*dirEntryInfo).entryInfo.longNameLocation.secterInClustor, (*dirEntryInfo).entryInfo.longNameEntryOffset);		
		}
	}
	
	return resultBuffer;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displayDirectoryAndFileEntryInfomation1(directoryAndFileEntryInformation *dirEntryInfo)
{
																					resetGlcd();
																					sprintf(g_glcdBuf, "S.N:%s,E:%s", (*dirEntryInfo).dirStructure.dirName.simple, (*dirEntryInfo).dirStructure.dirName.extension);
																					putStringInGlcdAtPage(PAGE1, g_glcdBuf);

																					sprintf(g_glcdBuf, "LongName");
																					putStringInGlcdAtPage(PAGE2, g_glcdBuf);

																					sprintf(g_glcdBuf, "%s", (*dirEntryInfo).dirStructure.dirName.fullName);
																					putStringInGlcdAtPage(PAGE3, g_glcdBuf);

																					sprintf(g_glcdBuf, "CS:0x%002x, LNEC:%d", (*dirEntryInfo).entryInfo.extensionNameChkSum, (*dirEntryInfo).entryInfo.extensionNameEntryCount);
																					putStringInGlcdAtPage(PAGE4, g_glcdBuf);
																					
																					sprintf(g_glcdBuf, "locatedFirstClustor");
																					putStringInGlcdAtPage(PAGE5, g_glcdBuf);

																					sprintf(g_glcdBuf, "0x%000000008lx", (*dirEntryInfo).entryInfo.location.clustor);
																					putStringInGlcdAtPage(PAGE6, g_glcdBuf);																				
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displayDirectoryAndFileEntryInfomation2(directoryAndFileEntryInformation *dirEntryInfo)
{
																					resetGlcd();
																					sprintf(g_glcdBuf, "indicate:0x%000000008lx", (*dirEntryInfo).dirStructure.otherInfo.indicateFirstClustor);
																					putStringInGlcdAtPage(PAGE1, g_glcdBuf);

																					sprintf(g_glcdBuf, "EntryNo:0x%002x", (*dirEntryInfo).entryInfo.entryNumberOrOffset);
																					putStringInGlcdAtPage(PAGE2, g_glcdBuf);

																					sprintf(g_glcdBuf, "clustor:0x%000000008lx", (*dirEntryInfo).entryInfo.location.clustor);
																					putStringInGlcdAtPage(PAGE3, g_glcdBuf);

																					sprintf(g_glcdBuf, "secterInC. 0d%d", (*dirEntryInfo).entryInfo.location.secterInClustor);
																					putStringInGlcdAtPage(PAGE4, g_glcdBuf);

																					sprintf(g_glcdBuf, "size 0x%ld", (*dirEntryInfo).dirStructure.otherInfo.fileSize);
																					putStringInGlcdAtPage(PAGE5, g_glcdBuf);

																					sprintf(g_glcdBuf, "attribute 0x%002x", (*dirEntryInfo).dirStructure.otherInfo.attribute);
																					putStringInGlcdAtPage(PAGE6, g_glcdBuf);

																					//sprintf(g_glcdBuf ,"empty C.L 0x%000000008lx", findEmptyClustor(&sdCardInfo, &clustor, (*dirEntryInfo).dirStructure.otherInfo.indicateFirstClustor));
																					//putStringInGlcdAtPage(PAGE7, g_glcdBuf);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displayDirectoryAndFileEntryInfomation3(directoryAndFileEntryInformation *dirEntryInfo)
{
																					resetGlcd();
																					sprintf(g_glcdBuf, "clustorNO:0x%000000008lx", (*dirEntryInfo).entryInfo.location.clustor);
																					putStringInGlcdAtPage(PAGE1, g_glcdBuf);

																					sprintf(g_glcdBuf, "secterNO.:0x%002d", (*dirEntryInfo).entryInfo.location.secterInClustor);
																					putStringInGlcdAtPage(PAGE2, g_glcdBuf);

																					sprintf(g_glcdBuf, "EntryOffs:0x%002d", (*dirEntryInfo).entryInfo.entryNumberOrOffset);
																					putStringInGlcdAtPage(PAGE3, g_glcdBuf);

																					sprintf(g_glcdBuf, "LongCluNO:0x%000000008lx", (*dirEntryInfo).entryInfo.longNameLocation.clustor);
																					putStringInGlcdAtPage(PAGE5, g_glcdBuf);

																					sprintf(g_glcdBuf, "LongSeNO.:0x%002d", (*dirEntryInfo).entryInfo.longNameLocation.secterInClustor);
																					putStringInGlcdAtPage(PAGE6, g_glcdBuf);

																					sprintf(g_glcdBuf, "LongEntOf:0x%002d", (*dirEntryInfo).entryInfo.longNameEntryOffset);
																					putStringInGlcdAtPage(PAGE7, g_glcdBuf);

																					//sprintf(g_glcdBuf ,"empty C.L 0x%000000008lx", findEmptyClustor(&sdCardInfo, &clustor, (*dirEntryInfo).dirStructure.otherInfo.indicateFirstClustor));
																					//putStringInGlcdAtPage(PAGE7, g_glcdBuf);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Text file and other paste information*/
const char EndOfTextFile[]={0x0d, 0x0a, 0x00};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char putPasteWritingInFile(fat32Info *diskInfo, clustorData *clustor, directoryAndFileEntryInformation *physicalDirLocationInfo, lastFileAccessInfo *lastestClustorInfo, char *pastedString, char *pastedTailString)
{
	/*
	upgrade idea
	sum pastedString and pastedTailString.
	*/
	/*empty file file is not file exception and start*/
	if( (*physicalDirLocationInfo).dirStructure.otherInfo.indicateFirstClustor == 0) return -1;
	else if(((*physicalDirLocationInfo).dirStructure.otherInfo.attribute&ATTR_DIR_FILE_MASK)!=ATTR_ARCHIVE) return -2;
	/*empty file exception end*/

	/*to copy string from buffer to sd card buffer variables definition.*/
	CLUSTOR_LOCATION writingLocationClustor;
	if((*lastestClustorInfo).fileIndicateClustor!=(*physicalDirLocationInfo).dirStructure.otherInfo.indicateFirstClustor)
	{
		writingLocationClustor = findFilesLastestClustor(diskInfo, clustor, (*physicalDirLocationInfo).dirStructure.otherInfo.indicateFirstClustor);
		(*lastestClustorInfo).fileIndicateClustor=(*physicalDirLocationInfo).dirStructure.otherInfo.indicateFirstClustor;
		(*lastestClustorInfo).lastFoundedClustor=writingLocationClustor;
	}
	else
	{
		writingLocationClustor = findFilesLastestClustor(diskInfo, clustor, (*lastestClustorInfo).lastFoundedClustor);
		(*lastestClustorInfo).lastFoundedClustor=writingLocationClustor;
	}

	
	unsigned long writingLocationSecterInClustor = findFilesLastestLocationInClustor(diskInfo, clustor, (*physicalDirLocationInfo).dirStructure.otherInfo.fileSize)/(*diskInfo).bytesPerSecter;//start number of secter in clustor is 0.
	char *writingPositionInSdCard = (*clustor).secterData.data;

	
	if( (*diskInfo).secterPerClustor <= writingLocationSecterInClustor)
	{//file size multiple exception
		writingPositionInSdCard+=SD_DATA_BUFFER_SIZE;
	}
	else
	{
		writingPositionInSdCard+=(findFilesLastestLocationInClustor(diskInfo, clustor, (*physicalDirLocationInfo).dirStructure.otherInfo.fileSize)%(*diskInfo).bytesPerSecter);//searching and loaded target Clustor.//input charactor location
	}

																						
//	char *testWritingAddedTailStringPointer = pastedTailString;
	const unsigned long writingStringOccupiedSizeMain=strlen(pastedString);
	const unsigned long writingStringOccupiedSizeTail=strlen(pastedTailString);
	unsigned long writingStringOccupiedRestSecterCount;//=writingStringOccupiedSize/SD_DATA_BUFFER_SIZE;
	unsigned int restOfWritingStringLengthAtLastestSecter;//=writingStringOccupiedSize%SD_DATA_BUFFER_SIZE;

	readSecterInClustor(diskInfo, clustor, writingLocationClustor, writingLocationSecterInClustor);


	if((writingPositionInSdCard+writingStringOccupiedSizeMain+writingStringOccupiedSizeTail)<(*clustor).secterData.data+SD_DATA_BUFFER_SIZE)//writing secter have enough empty bytes to write?
	{//yes{//yes//just copy from common buffer to sd card buffer.
		strncpy(writingPositionInSdCard, pastedString, writingStringOccupiedSizeMain);

		pastedString+=writingStringOccupiedSizeMain;
		writingPositionInSdCard+=writingStringOccupiedSizeMain;//have to move sdcardBuffer first.
		

		strncpy(writingPositionInSdCard, pastedTailString, writingStringOccupiedSizeTail);
		pastedTailString+=writingStringOccupiedSizeTail;
		writingPositionInSdCard+=writingStringOccupiedSizeTail;

		writeSecterInClustor(diskInfo, clustor, writingLocationClustor, writingLocationSecterInClustor);
	}
	else
	{//no//find next secter in same clustor or next clustor is empty.
		if( (writingPositionInSdCard+writingStringOccupiedSizeMain)<((*clustor).secterData.data+SD_DATA_BUFFER_SIZE) )//writingStringOccupiedSize=strlen(g_strBuf.loc)+strlen(pastedTailString);
		{
			strncpy(writingPositionInSdCard, pastedString, writingStringOccupiedSizeMain);
			pastedString+=writingStringOccupiedSizeMain;
			writingPositionInSdCard+=writingStringOccupiedSizeMain;

			/*tail is 3 case. Tail cannot input buffer(not empty space) or tail is cutout.*/
			if( (((*clustor).secterData.data+SD_DATA_BUFFER_SIZE)-writingPositionInSdCard)>0 )
			{
				strncpy(writingPositionInSdCard, pastedTailString, (((*clustor).secterData.data+SD_DATA_BUFFER_SIZE)-writingPositionInSdCard));
				pastedTailString+=(((*clustor).secterData.data+SD_DATA_BUFFER_SIZE)-writingPositionInSdCard);
				writingPositionInSdCard+=(((*clustor).secterData.data+SD_DATA_BUFFER_SIZE)-writingPositionInSdCard);
			}
		}
		else
		{
			strncpy(writingPositionInSdCard, pastedString, (((*clustor).secterData.data+SD_DATA_BUFFER_SIZE)-writingPositionInSdCard));
			pastedString+=(((*clustor).secterData.data+SD_DATA_BUFFER_SIZE)-writingPositionInSdCard);
			writingPositionInSdCard+=(((*clustor).secterData.data+SD_DATA_BUFFER_SIZE)-writingPositionInSdCard);
		}
		
		writeSecterInClustor(&sdCardInfo, clustor, writingLocationClustor, writingLocationSecterInClustor);
		/*because secter is full,so increase loaded sectre in clustor*/
		writingLocationSecterInClustor++;//writingLocationSecterInClustor have 1, next secter is empty but, it have 2 next clustor is not exist when secter per clustor is 2.
		/*because data that can be insert at lastest secter of target files, next step is find empty next secter in same clustor or clustor.*/

		if((strlen(pastedString)+strlen(pastedTailString))!=0)
		{
			restOfWritingStringLengthAtLastestSecter=(strlen(pastedString)+strlen(pastedTailString))%SD_DATA_BUFFER_SIZE;//calculate string length of lastest secter.
			writingStringOccupiedRestSecterCount=(strlen(pastedString)+strlen(pastedTailString))/SD_DATA_BUFFER_SIZE;//calculate writing secter number, but not this operating is ignore rest value.
			writingPositionInSdCard=(*clustor).secterData.data;//reset buffer pointer.

			if(restOfWritingStringLengthAtLastestSecter!=0)//case 2. just one secter is not full.
			{
				/*
					rest data have clustor case is 2.
					1. secter is full of data accurately.
					2. some secter is full but, any secter is not full. Otherwise, just one secter is not full.
				*/
				writingStringOccupiedRestSecterCount++;
			}

			while(writingStringOccupiedRestSecterCount)
			{
				if(writingLocationSecterInClustor<sdCardInfo.secterPerClustor)//secter in clustor is 0 or 1. secter had clustor -2,
				{//after write secter, increase secterInClustor variable.
					while(writingPositionInSdCard<=((*clustor).secterData.data+SD_DATA_BUFFER_SIZE))
					{
						if(*(pastedString)!=0)
						{
							*(writingPositionInSdCard)=*(pastedString);
							pastedString++;
						}
						else
						{
							if(*pastedTailString != 0)
							{
								*(writingPositionInSdCard)=*(pastedTailString);
								pastedTailString++;
							}
							else
							{
								*(writingPositionInSdCard)=0;
							}

						
						}
						writingPositionInSdCard++;
					
					}//escape while loop, when sd card buffer is full or common buffer is ended.

					writeSecterInClustor(diskInfo, clustor, writingLocationClustor, writingLocationSecterInClustor);
					writingPositionInSdCard=(*clustor).secterData.data;//

					writingLocationSecterInClustor++;
					writingStringOccupiedRestSecterCount--;
				}
				else
				{//find emtpty clustor. end set secter in clustor reset(set to zero).
					writingLocationClustor=writeNextClustor(diskInfo, clustor, writingLocationClustor, findEmptyClustor(diskInfo, clustor, writingLocationClustor));

					writingLocationSecterInClustor=0;
				}
			}
		}
	}
	(*lastestClustorInfo).lastFoundedClustor=(*clustor).locatedClustor;
	/*
		below code just write when secter is full and tail of data
		case is 2, one is wrote data size is small then sd card buffer size,
		another is wrote data size is same to sd card buffer size.
	*/
	/*
		writingStringOccupiedRestSecterCount is secter number will be wrote. so, it is decrease after executation of writeSecterInClustor.
		writingLocationSecterInClustor is secter number in target clustor. so, it is increase after executation of writeSecterInClustor.
		don't confuse both variables.
	*/

/*to Test after text file write, add file size start*/
	(*physicalDirLocationInfo).dirStructure.otherInfo.fileSize+=(writingStringOccupiedSizeMain+writingStringOccupiedSizeTail);

	readSecterInClustor(&sdCardInfo, clustor, (*physicalDirLocationInfo).entryInfo.location.clustor, (*physicalDirLocationInfo).entryInfo.location.secterInClustor);

	parsing32BitsToLittleEndian((*clustor).secterData.data+(*physicalDirLocationInfo).entryInfo.entryNumberOrOffset+DIR_FILESIZE ,(*physicalDirLocationInfo).dirStructure.otherInfo.fileSize);

	/*!!!!if varing lastest access time, added this location...*/
	writeSdcard(WRITE_BLOCK, &((*clustor).secterData), (*clustor).locatedClustorStartPhysicalSecter+(*clustor).secterInClustor, 512, 1);
	
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char putPasteWritingInFileIfNotCreateNewDirEntry(fat32Info *diskInfo, clustorData *clustor, directoryAndFileEntryInformation *physicalDirLocationInfo, lastFileAccessInfo *lastestClustorInfo, CLUSTOR_LOCATION targetClustor, char *fileName, char *pastedString, char *pastedTailString)
{
	if(findDirEntryIfNotCreateNewDirEntry(diskInfo, clustor, physicalDirLocationInfo, targetClustor, ATTR_ARCHIVE, fileName))
	{
		return -1;
	}
	return putPasteWritingInFile(diskInfo, clustor, physicalDirLocationInfo, lastestClustorInfo, pastedString, pastedTailString);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* saved file and saved data name format*/
/*
	file name
	yy-MM-dd.txt
	
	data
	hhmmss(6 characters),deviceNumber(4 characters),value1(4 characters),value2,value3,value4,value5,value6,value7,value8(\r\n)
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*saved file parsing*/
#define ANALOG_DATA_START	12
#define ANALOG_DATA_VALUE_LENGTH	4
#define ANALOG_DATA_OFFSET	(ANALOG_DATA_VALUE_LENGTH+1)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*analog data display*/
void analogDataDisplay(char *str)
{
	char strBuffer[ANALOG_DATA_VALUE_LENGTH+1]={0};
	unsigned char portNum=0;
	unsigned char displayPageNumber=5;

	
	sprintf(g_glcdBuf, "Analog %c", *(str+10));
	putStringInGlcdAtPage(PAGE0+1, g_glcdBuf);
	
	sprintf(g_glcdBuf, "Time:%c%c:%c%c:%c%c", *(str+0), *(str+1), *(str+2), *(str+3), *(str+4), *(str+5));
	putStringInGlcdAtPage(PAGE0+2, g_glcdBuf);

	for(portNum=0; portNum<8; portNum+=2)
	{
		strncpy(strBuffer, str+12+(portNum*ANALOG_DATA_OFFSET), ANALOG_DATA_VALUE_LENGTH);
		*(strBuffer+ANALOG_DATA_VALUE_LENGTH)=0;
		sprintf(g_glcdBuf, "PORT%d:%s", portNum+1, strBuffer);
		
		strncpy(strBuffer, str+12+((portNum+1)*ANALOG_DATA_OFFSET), ANALOG_DATA_VALUE_LENGTH);
		*(strBuffer+ANALOG_DATA_VALUE_LENGTH)=0;
		sprintf(g_glcdBuf, "%s,PORT%d:%s", g_glcdBuf, portNum+2, strBuffer);
		
	putStringInGlcdAtPage(PAGE0+4+(portNum/2), g_glcdBuf);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*when use readSecterInClustor function first, MSB in clutorData at secterData must setted zero.*/
char savedDataFileInfoParseFromSectorInClustor(fat32Info *diskInfo, clustorData *searchingSecterBuffer, simpleDirectoryAndFileEntryInfomation *p, CLUSTOR_LOCATION startClustor)
{
	unsigned int count=0;
	char *str;
	
	if(((*p).dirStructure.otherInfo.indicateFirstClustor == 0)||(*p).dirStructure.otherInfo.fileSize == 0)
	{
		putStringInGlcdAtPage(PAGE1, "file is empty.");
		nextSequence();
		return -1;
	}
	
	/*read file name part start*/
	if((*p).entryInfo.extensionNameEntryCount!=0)
	{
		memset(g_strBuf.dat, 0x00, STRING_BUFFER_SIZE);

		(*searchingSecterBuffer).locatedClustor=(*p).entryInfo.longNameLocation.clustor;
		(*searchingSecterBuffer).secterInClustor=(*p).entryInfo.longNameLocation.secterInClustor;

		checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);//if want check wrong fat table, added exception process

		readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);

		str = (*searchingSecterBuffer).secterData.data + (*p).entryInfo.longNameEntryOffset;
		for(count=(*p).entryInfo.extensionNameEntryCount; count!=0; count--)
		{
			abstractDirLongNameFromDirectoryEntry(str, g_strBuf.dat);
			str+=DIR_DISCRIPTION_LENGTH;
			if( !(str<(*searchingSecterBuffer).secterData.data+SD_DATA_BUFFER_SIZE) )
			{
				(*searchingSecterBuffer).secterInClustor++;

				if( !((*searchingSecterBuffer).secterInClustor < (*diskInfo).secterPerClustor) )//finding secter is lastest secter of clustor?
				{//lastest secter of clustor. loading next clustor
					(*searchingSecterBuffer).locatedClustor=(*searchingSecterBuffer).nextClustor;
					checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);
					(*searchingSecterBuffer).secterInClustor=0;
				}

				readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);				
				str = (*searchingSecterBuffer).secterData.data;
			}
		}
	}
	else
	{
		memset(g_strBuf.dat, 0x00, STRING_BUFFER_SIZE);

		(*searchingSecterBuffer).locatedClustor=(*p).entryInfo.location.clustor;
		(*searchingSecterBuffer).secterInClustor=(*p).entryInfo.location.secterInClustor;

		readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);

		str = (*searchingSecterBuffer).secterData.data + (*p).entryInfo.entryNumberOrOffset;

		for(count=0; count<8; count++)
		{
			if(*(str+count) == DIR_NAME_EMPTY_DATA) break;
			*(g_strBuf.dat+count)=*(str+count);

		}
		
		*(g_strBuf.dat+count)='.';
		count++;
		strncpy(g_strBuf.dat+count, str+DIR_SIMPLE_NAME_MAXIMUM_LENGTH , DIR_EXTENSION_MAXUMUM_LENGTH);//general name copy

		checkAndConvertSimpleNameStringForm(g_strBuf.dat, count+DIR_EXTENSION_MAXUMUM_LENGTH+1);
	}
	/*read file name part end*/

	/*if need file name processing, add code this. file name is loaded g_strBuf.dat. start*/
	putStringInGlcdAtPage(PAGE0+0, g_strBuf.dat);
	/*if need file name processing, add code this. file name is loaded g_strBuf.dat. end*/
	

	unsigned long readFileSize=0;//This variable is same to file size.

	(*searchingSecterBuffer).locatedClustor = (*p).dirStructure.otherInfo.indicateFirstClustor;
	(*searchingSecterBuffer).secterInClustor = 0;

	checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);
	readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);	
	
	/*if skipped data is exist, add code this. Data Info length must not exceed 512 bytes.*/
	str = (*searchingSecterBuffer).secterData.data;
	count = 0;
	while(*(str+count)=='[')
	{
		/*if need to process every non saved data, added code here.*/
		while(*(str+count-1)!=0x0a)
		{
			count++;
		}
	}
	const unsigned int dataFileInfoLength=count;
	str+=count;//pointer move
	readFileSize=count;
	count=0;
	
	/*
		After data is displaied, pointer is indicating end of buffer. Below is example.
		<- is pointer location.
		/cr is carriage return.
		/nl is line feed.
		
		before, data infomation not process.
		[<-DataLogger Finfotech]
		[Date:2014:01:24]
		000423,ADC0,1604,1663,2205,1473,2257,1088,3214,2765/cr/nl
		006897,ADC1,3939,2228,2878,0270,1831,2412,0534,3942/cr/nl
		003289,ADC0,1590,1649,2185,1460,2237,1078,3186,2736/cr/nl
		003114,ADC1,3946,2231,2881,0270,1833,2415,0535,3946/cr/nl
		006018,ADC0,1592,1651,2187,1459,2237,1079,3189,2740/cr/nl
	
		before, display data.
		[DataLogger Finfotech]
		[Date:2014:01:24]
		0<-00423,ADC0,1604,1663,2205,1473,2257,1088,3214,2765/cr/nl
		006897,ADC1,3939,2228,2878,0270,1831,2412,0534,3942/cr/nl
		003289,ADC0,1590,1649,2185,1460,2237,1078,3186,2736/cr/nl
		003114,ADC1,3946,2231,2881,0270,1833,2415,0535,3946/cr/nl
		006018,ADC0,1592,1651,2187,1459,2237,1079,3189,2740/cr/nl
	
		after 3 line data is displaied.
		[DataLogger Finfotech]
		[Date:2014:01:24]
		000423,ADC0,1604,1663,2205,1473,2257,1088,3214,2765/cr/nl
		0<-06897,ADC1,3939,2228,2878,0270,1831,2412,0534,3942/cr/nl
		003289,ADC0,1590,1649,2185,1460,2237,1078,3186,2736/cr/nl
		003114,ADC1,3946,2231,2881,0270,1833,2415,0535,3946/cr/nl
		006018,ADC0,1592,1651,2187,1459,2237,1079,3189,2740/cr/nl
	*/
	
	char switchBuffer = '2';
	
	char *displayPointer;
	
	do
	{
		switch(switchBuffer)
		{
			case '0'://Reverse
				/* readFileSize is same mean that fileSize. */
				/* if updated string size is longer then file size not updated at GLCD. */
				
				/*
				bufferDirection is in (struct stringBuffer) have any identification values,
				from identification values MCU know how to calculate buffer string length to read before data.

				buffer size (is already read from sd card) calculating.
				1. case Forward. ('F')
					0 1 2 3 4 5 6 7 8 9 10(buffer offset)
					|string start(offset 0)
					            |string end(offset 6)
								  |buffer pointer indicate point is 7(offset) and there is reserved to 0.
					To calculate string length is buffered in buffer, pointer address minus 1st buffer address(offset 0).
					And bufferPointer is in structure of [struct stringBuffer].
					
					if bufferDirection have unicode 'F', bufferPointer indicate lastest buffer location.
				
				2. case Reverse. ('R')
					0 1 2 3 4 5 6 7 8 9 10(reserved, it have 0) | 11(invalid memory region)//string length is 8.
									  |string start(offset 9), offset 10 is reserved to end of array.
					    |string end(offset 2)
						|buffer pointer indicate point is 1(offset)
					To calculate string length is buffered in buffer, 11th buffer address(offset 10) minus pointer address.
					And bufferPointer is in structure of [struct stringBuffer].

					if bufferDirection have unicode 'R', bufferPointer indicate 1st buffer location.
				*/
				if(g_strBuf.dir == 'F')
				{
					count=(g_strBuf.loc-g_strBuf.dat);
				}
				else if(g_strBuf.dir == 'R')
				{
					count=(g_strBuf.dat+STRING_BUFFER_SIZE-2)-g_strBuf.loc;
				}
				
				// g_strBuf.loc=g_strBuf.dat;
				/*
				classify case.
				1. no rest
					1-1. when values 0.
						clustor is zero.
						secter is zero.
						char pointer is zero.
					1-2. perfectly divide, so no rest.
				2. occur rest.
				*/
				
				if((count<readFileSize))/*when data buffer is indicate data.*/
				{
					readFileSize-=count;//before data set length

					if((dataFileInfoLength<readFileSize))
					{
						*(g_strBuf.dat + STRING_BUFFER_SIZE - 1) = 0;//End of char array reserved bytes.
						g_strBuf.loc = (g_strBuf.dat+STRING_BUFFER_SIZE-2);//when calculate copied string length, (g_strBuf.dat+STRING_BUFFER_SIZE-2) is reference offset in reserve.
						g_strBuf.dir='R';

					
						str=((*searchingSecterBuffer).secterData.data+((readFileSize-1)%((CLUSTOR_LOCATION)(*diskInfo).bytesPerSecter)));//rest
						(*searchingSecterBuffer).secterInClustor=(((readFileSize-1)/((CLUSTOR_LOCATION)(*diskInfo).bytesPerSecter))%(*diskInfo).secterPerClustor);
						(*searchingSecterBuffer).locatedClustor=findNthClustor(diskInfo, searchingSecterBuffer, (*p).dirStructure.otherInfo.indicateFirstClustor, (readFileSize-1)/(((CLUSTOR_LOCATION)(*diskInfo).secterPerClustor)*((CLUSTOR_LOCATION)(*diskInfo).bytesPerSecter)));//return number of sum of clustor - 1
						
						readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);
						do/*pointer is move back. */
						{
							if(str<(*searchingSecterBuffer).secterData.data)
							{
							
								if((*searchingSecterBuffer).secterInClustor==0)//wrong??
								{
									if((*searchingSecterBuffer).locatedClustor != (*p).dirStructure.otherInfo.indicateFirstClustor)
									{
										(*searchingSecterBuffer).locatedClustor=findBeforeClustor(diskInfo, searchingSecterBuffer, (*p).dirStructure.otherInfo.indicateFirstClustor, (*searchingSecterBuffer).locatedClustor);//return number of sum of clustor - 1
										(*searchingSecterBuffer).secterInClustor=(*diskInfo).secterPerClustor-1;
									}
									else
									{
										break;
									}
								}
								else
								{
									(*searchingSecterBuffer).secterInClustor--;
								}
							
								readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);
								
								str=(*searchingSecterBuffer).secterData.data+SD_DATA_BUFFER_SIZE-1;
							}

							for(;!(str<(*searchingSecterBuffer).secterData.data);)
							{
								*(g_strBuf.loc)=*(str);//copy.

								if( (g_strBuf.loc != (g_strBuf.dat + STRING_BUFFER_SIZE - 2)) && (*(g_strBuf.loc)==0x0a) )//always 0x0a is located tail of data set.
								{
									break;
								}
								else
								{
									g_strBuf.loc--;
									str--;
								}
							}
							
							
							
						}
						while((((*searchingSecterBuffer).locatedClustor!=(*p).dirStructure.otherInfo.indicateFirstClustor)||((*searchingSecterBuffer).secterInClustor!=0))&&(*(g_strBuf.loc)!=0x0a));//direction is reverse.

						displayPointer = (g_strBuf.loc + 1);

						break;
					}
				}
				
				/*when buffer pointer is indicate data info.*/
				str=(*searchingSecterBuffer).secterData.data+dataFileInfoLength;
				(*searchingSecterBuffer).locatedClustor=(*p).dirStructure.otherInfo.indicateFirstClustor;
				(*searchingSecterBuffer).secterInClustor=0;
				readFileSize=dataFileInfoLength;
				
			case '2'://Forward
				/*Read file size is can't exceed file size.*/
				if((*p).dirStructure.otherInfo.fileSize<=readFileSize) continue;
				/*data abstract*/
				g_strBuf.loc=g_strBuf.dat;
				g_strBuf.dir='F';

				
				str=(*searchingSecterBuffer).secterData.data+(readFileSize%((CLUSTOR_LOCATION)(*diskInfo).bytesPerSecter));//rest
				(*searchingSecterBuffer).secterInClustor=((readFileSize/((CLUSTOR_LOCATION)(*diskInfo).bytesPerSecter))%(*diskInfo).secterPerClustor);
				(*searchingSecterBuffer).locatedClustor=findNthClustor(diskInfo, searchingSecterBuffer, (*p).dirStructure.otherInfo.indicateFirstClustor, readFileSize/(((CLUSTOR_LOCATION)(*diskInfo).secterPerClustor)*((CLUSTOR_LOCATION)(*diskInfo).bytesPerSecter)));//return number of sum of clustor - 1

				readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);				
				

				do////direction is forward.
				{
					//if MCU exceed 1 secter
					if(!(str<((*searchingSecterBuffer).secterData).data+SD_DATA_BUFFER_SIZE))
					{
						(*searchingSecterBuffer).secterInClustor++;

						if( !((*searchingSecterBuffer).secterInClustor < ((*diskInfo).secterPerClustor)) )//finding secter is lastest secter of clustor?
						{//lastest secter of clustor. loading next clustor
							(*searchingSecterBuffer).locatedClustor=(*searchingSecterBuffer).nextClustor;
							checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);

							(*searchingSecterBuffer).secterInClustor=0;
						}
						
						readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);				

						str=(*searchingSecterBuffer).secterData.data;
					}

					
					for(;str<(((*searchingSecterBuffer).secterData).data+SD_DATA_BUFFER_SIZE);str++)
					{
						*(g_strBuf.loc)=*(str);//copy.

						if(*(g_strBuf.loc-1)==0x0a)
						{
							*(g_strBuf.loc)=0;
							readFileSize+=(g_strBuf.loc-g_strBuf.dat);
							break;
						}
						g_strBuf.loc++;
					}
				}
				while((((*searchingSecterBuffer).nextClustor!=CLUSTOR_IS_END)||((*searchingSecterBuffer).secterInClustor<(*diskInfo).secterPerClustor))&&(*(g_strBuf.loc-1)!=0x0a));//direction is forward.

				displayPointer = (g_strBuf.dat);				
				break;
			
			default:
				continue;
		}


		sprintf(g_glcdBuf, "fileSize:0d%ld", readFileSize);
		putStringInGlcdAtPage(PAGE4, g_glcdBuf);
		
		count=0;
		/*classify data*/
		
		if(*(displayPointer+7)=='A')
		{
			analogDataDisplay(displayPointer);
		}
	}
	while((switchBuffer = nextSequence())!='4');/*variable str is must moved, so trace reverse direction, using another variable*/
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define SAVED_DATA_VERIFICTION_WORD	"[Finfotech Saved Data]"
const char SAVED_DATA_VERIFICTION_WORD[] = "[Finfotech Saved Data]";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////*using file browser2222222222222 start*//////////////////////////////
void getNextEntry(fat32Info *diskInfo, clustorData *searchingSecterBuffer, directoryAndFileEntryInformation *foundDirEntryInfo)
{

	(*foundDirEntryInfo).entryInfo.entryNumberOrOffset+=DIR_DISCRIPTION_LENGTH;
	if((*foundDirEntryInfo).entryInfo.entryNumberOrOffset < SD_DATA_BUFFER_SIZE)
	{
		return;
	}
	else
	{
		(*foundDirEntryInfo).entryInfo.entryNumberOrOffset=0;
	}

	(*foundDirEntryInfo).entryInfo.location.secterInClustor++;
	if( (*searchingSecterBuffer).secterInClustor < ((*diskInfo).secterPerClustor) )
	{
		return;
	}
	else
	{
		(*searchingSecterBuffer).secterInClustor=0;
	}
	
	(*foundDirEntryInfo).entryInfo.location.clustor = (*searchingSecterBuffer).nextClustor;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displayOneEntryComponent(unsigned char index, directoryStructure *dirStructure)
{
	unsigned char columnOffsetBuffer=FONT_5X8_OFFSET*2;
	//dir or file display

	if( ((*dirStructure).otherInfo.attribute&ATTR_DIR_FILE_MASK) ==  ATTR_DIRECTORY )
	{
		putStringInGlcdAtPage(PAGE0+index, "D:");
	}
	else if( ((*dirStructure).otherInfo.attribute&ATTR_DIR_FILE_MASK) ==  ATTR_ARCHIVE )
	{
		putStringInGlcdAtPage(PAGE0+index, "F:");		
	}
	else
	{
		putStringInGlcdAtPage(PAGE0+index, "  ");
	}
	
	if(strlen((*dirStructure).dirName.fullName) != 0)
	{
		putStringInGlcdAtPageUsingOffset(index, (*dirStructure).dirName.fullName, columnOffsetBuffer);//FONT_5X8_OFFSET*2;
	}
	else
	{
		putStringInGlcdAtPageUsingOffset(index, (*dirStructure).dirName.simple, columnOffsetBuffer);
		columnOffsetBuffer+=FONT_5X8_OFFSET*strlen((*dirStructure).dirName.simple);
		
		if(strlen((*dirStructure).dirName.extension) !=0)
		{
			putStringInGlcdAtPageUsingOffset(index, ".", columnOffsetBuffer);
			columnOffsetBuffer+=FONT_5X8_OFFSET;
			
			putStringInGlcdAtPageUsingOffset(index, (*dirStructure).dirName.extension, columnOffsetBuffer);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char displayAllLine(fat32Info *diskInfo, clustorData *searchingSecterBuffer, simpleDirStructureExtension *pageInfo)
{
	if((*pageInfo).validPageNumber < 0)
	{
		(*pageInfo).validPageNumber=0;
	}

	(*searchingSecterBuffer).locatedClustor=(*pageInfo).firstDirEntryInfo.location.clustor;
	(*searchingSecterBuffer).secterInClustor=(*pageInfo).firstDirEntryInfo.location.secterInClustor;
	
	
	char findPageNumber=0;
	char findLineNumber=0;	
	char *str;
	unsigned char i;
	

	checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);//if want check wrong fat table, added exception process

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
			if((*str==DIR_DELEDTED)||(*str==DIR_EMPTY))//deleted or empty entry
			{
				(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.longNameLocation.clustor=0;
				(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.longNameLocation.secterInClustor=-1;
				(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.longNameEntryOffset=-1;
				
				(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.extensionNameEntryCount=0;
				continue;
			}
			else if(*( (*pageInfo).findEntry.dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH)!=0)//if exceed maximum length of displaied string, passed entry.
			{
				(*( (*pageInfo).findEntry.dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH))--;
				continue;
			}
			else if( ( (*(str+DIR_ATTR_OFFSET)) & ATTRIBUTE_MASK ) == ATTRIBUTE_MASK )//encountered long name entry.
			{
				if(findPageNumber<(*pageInfo).validPageNumber)
				{//*(longName+LONG_NAME_MAXIMUM_LENGTH) = ((*(dirEntry))&LONG_NAME_NUMBER_MASK)-1-LONG_NAME_ENTRY_MAXIMUM_NUMBER;
					*((*pageInfo).findEntry.dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH) = (((*str)&LONG_NAME_NUMBER_MASK)-1);//to skip finding.
				}
				else
				{
					if( ( (*(str)) & LONG_NAME_LASTEST_MASK ) == LONG_NAME_LASTEST_VALID_VALUE )//save first long name entry location info.
					{
						(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.longNameLocation.clustor=(*searchingSecterBuffer).locatedClustor;
						(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.longNameLocation.secterInClustor=(*searchingSecterBuffer).secterInClustor;
						(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.longNameEntryOffset=str-(*searchingSecterBuffer).secterData.data;
						
						(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.extensionNameEntryCount=(*(str)&LONG_NAME_NUMBER_MASK);
					}
				
					/* abstractDirLongNameFromDirectoryEntry is return -1 when long name entry number is exceed maximum displaied string.
					Finally, skip entry that exceeded entry, that can't present. */
					abstractDirLongNameFromDirectoryEntry(str, (*pageInfo).findEntry.dirStructure.dirName.fullName);
				}
				continue;
			}

			
			if(findPageNumber<(*pageInfo).validPageNumber)
			{
				findLineNumber++;
				if(!(findLineNumber<DIR_DISPLAY_NUMBER))
				{
					findPageNumber++;
					findLineNumber=0;
				}
				continue;
			}
			//different name is filtered above condition!
			dirSimpleNameAbstractFromDirectoryEntry(str, &((*pageInfo).findEntry.dirStructure.dirName));

			/*!!!!if varing lastest access time, added function that abstract lastest access time from dir entry, this location...*/
			dirOtherInfoAbstractFromDirectoryEntry(str, &((*((*pageInfo).dirEntry+findLineNumber)).dirStructure.otherInfo) );
			dirOtherInfoAbstractFromDirectoryEntry(str, &((*pageInfo).findEntry.dirStructure.otherInfo) );

			(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.entryNumberOrOffset=str-(*searchingSecterBuffer).secterData.data;

			(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.location.clustor=(*searchingSecterBuffer).locatedClustor;

			(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.location.secterInClustor=(*searchingSecterBuffer).secterInClustor;

			displayOneEntryComponent(findLineNumber, &((*pageInfo).findEntry.dirStructure));

			//reset findEntry info start//
			*((*pageInfo).findEntry.dirStructure.dirName.simple)=0;
			*((*pageInfo).findEntry.dirStructure.dirName.extension)=0;
			*((*pageInfo).findEntry.dirStructure.dirName.fullName)=0;
			(*pageInfo).findEntry.dirStructure.otherInfo.attribute=0;
			//reset findEntry info end//
			
			findLineNumber++;
			
			if(!(findLineNumber<DIR_DISPLAY_NUMBER))
			{
				(*pageInfo).validPageNumber = findPageNumber;
				(*pageInfo).lastLineNumber = findLineNumber;
				return 0;
			}
		}
		(*searchingSecterBuffer).secterInClustor++;
	}
	while( ((*searchingSecterBuffer).nextClustor != CLUSTOR_IS_END) || ((*searchingSecterBuffer).secterInClustor<((*diskInfo).secterPerClustor)) );
	(*pageInfo).findEntry.entryInfo.location.secterInClustor=(*diskInfo).secterPerClustor-1;

	/*there is last page of dir list.*/		
	//blank dir, print blank start//	
	(*pageInfo).lastLineNumber = findLineNumber;
	
	while(findLineNumber < DIR_DISPLAY_NUMBER)
	{
		(*((*pageInfo).dirEntry+findLineNumber)).dirStructure.otherInfo.indicateFirstClustor=0;
		(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.entryNumberOrOffset=-1;
		(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.location.clustor=0;
		(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.location.secterInClustor=-1;

		displayOneEntryComponent(findLineNumber, &((*pageInfo).findEntry.dirStructure));

		//reset extension entry info.
		(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.longNameLocation.clustor=-1;
		(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.longNameLocation.secterInClustor=-1;
		(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.longNameEntryOffset=-1;
		
		(*((*pageInfo).dirEntry+findLineNumber)).entryInfo.extensionNameEntryCount=0;
		
		findLineNumber++;
	}
	(*pageInfo).validPageNumber=findPageNumber;	
	//blank dir, print blank end//
	//if target is not found, (*pageInfo).findEntry.entryInfo.location.clustor is indicate lastest clustor, that is found by this function.
	return -1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void testDisplayIndicateClustorInfo(simpleDirectoryAndFileEntryInfomation *dirEntry)
{
	unsigned char i;
	for(i=0; i<DIR_DISPLAY_NUMBER; i++)
	{
		sprintf(g_glcdBuf, "0x%lx", (*(dirEntry+i)).dirStructure.otherInfo.indicateFirstClustor);
		putStringInGlcdAtPageUsingOffset(i, g_glcdBuf, FONT_5X8_OFFSET*15);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void fileBrowser(fat32Info *diskInfo, clustorData *searchingSecterBuffer, simpleDirStructureExtension *browserData)
{
	memset(&fileBrowserData, 0x00, sizeof(simpleDirStructureExtension));

	(*browserData).firstDirEntryInfo.location.clustor = sdCardInfo.rootClustor;
	(*browserData).firstDirEntryInfo.location.secterInClustor = 0;
	(*browserData).firstDirEntryInfo.entryNumberOrOffset = 0;

	(*browserData).validPageNumber=0;
	(*browserData).selectLineNumber=0;//valid range 0~7, 1st(top) line is 0, 7th(bottom) line is 7
	signed int pageNumberBuffer=0;
	signed char lineNumberBuffer=0;
	char switchBuffer;

	
	displayAllLine(diskInfo, searchingSecterBuffer, browserData);

	reversePage((*browserData).selectLineNumber);

	while((switchBuffer=nextSequence()))
	{
		lineNumberBuffer=(*browserData).selectLineNumber;
		pageNumberBuffer=(*browserData).validPageNumber;
		switch(switchBuffer)//Which button is pressed?
		{
			case '1'://down
				(*browserData).selectLineNumber++;
				break;
			
			case '5'://up
				(*browserData).selectLineNumber--;
				break;
			case '0':
			case '2':
				/*find clustor...*/
				if((*((*browserData).dirEntry+(*browserData).selectLineNumber)).dirStructure.otherInfo.attribute == ATTR_DIRECTORY)//dir
				{
					if( ( (*((*browserData).dirEntry+(*browserData).selectLineNumber)).dirStructure.otherInfo.indicateFirstClustor < 2 ) || ( (*((*browserData).dirEntry+(*browserData).selectLineNumber)).dirStructure.otherInfo.indicateFirstClustor > (CLUSTOR_IS_END-1) ) )
					{
						(*browserData).firstDirEntryInfo.location.clustor = sdCardInfo.rootClustor;
					}
					else if( ( (*((*browserData).dirEntry+(*browserData).selectLineNumber)).dirStructure.otherInfo.indicateFirstClustor ==  (*browserData).firstDirEntryInfo.location.clustor ) )
					{
						(*browserData).firstDirEntryInfo.location.secterInClustor = 0;
					}
					else
					{
						(*browserData).firstDirEntryInfo.location.clustor = (*((*browserData).dirEntry+(*browserData).selectLineNumber)).dirStructure.otherInfo.indicateFirstClustor;
						(*browserData).firstDirEntryInfo.location.secterInClustor = 0;
					}
				
					(*browserData).validPageNumber=0;
					(*browserData).selectLineNumber=0;//valid range 0~7, 1st(top) line is 0, 7th(bottom) line is 7
					displayAllLine(diskInfo, searchingSecterBuffer, browserData);

					reversePage((*browserData).selectLineNumber);
					continue;
				}
				else if((*((*browserData).dirEntry+(*browserData).selectLineNumber)).dirStructure.otherInfo.attribute == ATTR_ARCHIVE)//file
				{
					savedDataFileInfoParseFromSectorInClustor(diskInfo, searchingSecterBuffer, (*browserData).dirEntry+(*browserData).selectLineNumber, (*((*browserData).dirEntry+(*browserData).selectLineNumber)).dirStructure.otherInfo.indicateFirstClustor);

					displayAllLine(diskInfo, searchingSecterBuffer, browserData);//display reset(reverse line)
					reversePage((*browserData).selectLineNumber);
					continue;
				}
				else//other...
				{
					(*browserData).firstDirEntryInfo.location.clustor = sdCardInfo.rootClustor;
					
					(*browserData).validPageNumber=0;
					(*browserData).selectLineNumber=0;//valid range 0~7, 1st(top) line is 0, 7th(bottom) line is 7
					displayAllLine(diskInfo, searchingSecterBuffer, browserData);

					reversePage((*browserData).selectLineNumber);
					continue;
				}
				
			default:
				continue;
		}

		
		if((*browserData).selectLineNumber < 0)//page up
		{
			(*browserData).validPageNumber--;
			displayAllLine(diskInfo, searchingSecterBuffer, browserData);//display reset(reverse line)

			
			if((pageNumberBuffer - (*browserData).validPageNumber) !=0 )//move page
			{
				(*browserData).selectLineNumber=DIR_DISPLAY_NUMBER-1;
			}
			else//top page
			{
				(*browserData).selectLineNumber=0;
			}
			reversePage((*browserData).selectLineNumber);
		}
		else if(!((*browserData).selectLineNumber<(*browserData).lastLineNumber))//page down
		{
			(*browserData).validPageNumber++;
			displayAllLine(diskInfo, searchingSecterBuffer, browserData);//display reset(reverse line)
			
			if((pageNumberBuffer - (*browserData).validPageNumber) !=0 )//move page
			{
				(*browserData).selectLineNumber=0;
			}
			else//bottom page
			{
				(*browserData).selectLineNumber=(*browserData).lastLineNumber-1;
			}
			
			if((*browserData).selectLineNumber==((*browserData).lastLineNumber-1))
			{
				reversePage((*browserData).selectLineNumber);
			}
			reversePage((*browserData).selectLineNumber);
		}
		else if( (lineNumberBuffer-(*browserData).selectLineNumber) !=0)//samePage
		{
			reversePage(lineNumberBuffer);
			reversePage((*browserData).selectLineNumber);
			continue;
		}
	}
}
//////////////////////////////*using file browser2222222222222 end*//////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAXIMUM_CMD_ARGS_NUM	5
#define CMD_ARGC_SPACE	' '
#define CMD_ARGC_LINEFEED	'\r'
#define CMD_ARGC_RETURN	'\n'
void cmdArgsLocation(char **argc_l, char *cmd)
{
	char i=0;
	
	*(argc_l+i++)=cmd;
	while(((*cmd)!=CMD_ARGC_LINEFEED)||((*cmd)!=CMD_ARGC_RETURN))
	{
		if(!(i<MAXIMUM_CMD_ARGS_NUM))
		{
			*cmd=0;
			return;
		}
		if(*cmd==0)
		{
			for(;i<MAXIMUM_CMD_ARGS_NUM;i++)
			{
				*(argc_l+i)=cmd;
			}
			return;
		}
		else if(((*cmd)==CMD_ARGC_LINEFEED)||((*cmd)==CMD_ARGC_RETURN))
		{
			(*cmd)=0;
			for(;i<MAXIMUM_CMD_ARGS_NUM;i++)
			{
				*(argc_l+i)=cmd;
			}
			return;
		}
		else if((*cmd)==CMD_ARGC_SPACE)
		{
			(*cmd)=0;
			cmd++;
			*(argc_l+i)=cmd;
			i++;
		}
		else
		{
			cmd++;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
struct stringBuffer
{
	char buffer[STRING_BUFFER_SIZE];
	char *bufferPointer;
	char bufferDirection;
}typedef stringBuffer;
*/
char sendDirEntryInfoToUsart0(directoryAndFileEntryInformation *findEntryBuffer)
{
	if(((*findEntryBuffer).dirStructure.otherInfo.attribute&ATTR_DIRECTORY)==ATTR_DIRECTORY)
	{
		sendStringOnly("d ");
	}
	else
	{
		sendStringOnly("- ");
	}
	
	sprintf(g_strBuf.dat, "%10ld ", (*findEntryBuffer).dirStructure.otherInfo.fileSize);
	sendStringOnly(g_strBuf.dat);
	
	sprintf(g_strBuf.dat, "WT %4d/%2d/%2d,%2d:%2d:%2d ", (*findEntryBuffer).dirStructure.writeDateInfo.date.year+1980, (*findEntryBuffer).dirStructure.writeDateInfo.date.month, (*findEntryBuffer).dirStructure.writeDateInfo.date.day, (*findEntryBuffer).dirStructure.writeDateInfo.time.hour, (*findEntryBuffer).dirStructure.writeDateInfo.time.minute, (*findEntryBuffer).dirStructure.writeDateInfo.time.second);
	sendStringOnly(g_strBuf.dat);

	sprintf(g_strBuf.dat, "C.N:0x%000000008x ", (*findEntryBuffer).dirStructure.otherInfo.indicateFirstClustor);
	sendStringOnly(g_strBuf.dat);
	
	if((*findEntryBuffer).entryInfo.extensionNameEntryCount==0)
	{
		// sendStringOnly(" S:");
		sendStringOnly((*findEntryBuffer).dirStructure.dirName.simple);
		if(*((*findEntryBuffer).dirStructure.dirName.extension)!=0)
		{
			sendCharOnly('.');
			sendStringOnly((*findEntryBuffer).dirStructure.dirName.extension);

		}
	}
	else
	{
		// sprintf(g_strBuf.dat, " L%d:", (*findEntryBuffer).entryInfo.extensionNameEntryCount);
		// sendStringOnly(g_strBuf.dat);
		sendStringOnly((*findEntryBuffer).dirStructure.dirName.fullName);
	}
	SEND_COMMON();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char cmdDisplayDirList(fat32Info *diskInfo, clustorData *searchingSecterBuffer, directoryAndFileEntryInformation *findEntryBuffer, CLUSTOR_LOCATION cmdClustorLocation)
{
	char *str;

	(*searchingSecterBuffer).locatedClustor=cmdClustorLocation;
	(*searchingSecterBuffer).secterInClustor=0;

	
	*((*findEntryBuffer).dirStructure.dirName.simple)=0;
	*((*findEntryBuffer).dirStructure.dirName.extension)=0;
	*((*findEntryBuffer).dirStructure.dirName.fullName)=0;

	(*findEntryBuffer).entryInfo.longNameLocation.clustor=0;
	(*findEntryBuffer).entryInfo.longNameLocation.secterInClustor=-1;
	(*findEntryBuffer).entryInfo.longNameEntryOffset=-1;
	
	(*findEntryBuffer).entryInfo.extensionNameEntryCount=0;


	checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);//if want check wrong fat table, added exception process

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
			if((*str==DIR_DELEDTED)||(*str==DIR_EMPTY))//deleted or empty entry
			{
				(*findEntryBuffer).entryInfo.longNameLocation.clustor=0;
				(*findEntryBuffer).entryInfo.longNameLocation.secterInClustor=-1;
				(*findEntryBuffer).entryInfo.longNameEntryOffset=-1;
				
				(*findEntryBuffer).entryInfo.extensionNameEntryCount=0;
				continue;
			}
			else if(*( (*findEntryBuffer).dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH)!=0)//if exceed maximum length of displaied string, passed entry.
			{
				(*( (*findEntryBuffer).dirStructure.dirName.fullName+LONG_NAME_MAXIMUM_LENGTH))--;
				continue;
			}
			else if( ( (*(str+DIR_ATTR_OFFSET)) & ATTRIBUTE_MASK ) == ATTRIBUTE_MASK )//encountered long name entry.
			{
				if( ( (*(str)) & LONG_NAME_LASTEST_MASK ) == LONG_NAME_LASTEST_VALID_VALUE )//save first long name entry location info.
				{
					(*findEntryBuffer).entryInfo.longNameLocation.clustor=(*searchingSecterBuffer).locatedClustor;
					(*findEntryBuffer).entryInfo.longNameLocation.secterInClustor=(*searchingSecterBuffer).secterInClustor;
					(*findEntryBuffer).entryInfo.longNameEntryOffset=str-(*searchingSecterBuffer).secterData.data;
					
					(*findEntryBuffer).entryInfo.extensionNameEntryCount=(*(str)&LONG_NAME_NUMBER_MASK);
				}
			
				/* abstractDirLongNameFromDirectoryEntry is return -1 when long name entry number is exceed maximum displaied string.
				Finally, skip entry that exceeded entry, that can't present. */
				abstractDirLongNameFromDirectoryEntry(str, (*findEntryBuffer).dirStructure.dirName.fullName);//if exc

				continue;
			}

			
			//different name is filtered above condition!
			dirSimpleNameAbstractFromDirectoryEntry(str, &((*findEntryBuffer).dirStructure.dirName));

			/*!!!!if varing lastest access time, added function that abstract lastest access time from dir entry, this location...*/
			dirOtherInfoAbstractFromDirectoryEntry(str, &((*findEntryBuffer).dirStructure.otherInfo) );
			dirOtherInfoAbstractFromDirectoryEntry(str, &((*findEntryBuffer).dirStructure.otherInfo) );

			dirDateAndTimeInfoParseFromDirectoryEntry(str, &((*findEntryBuffer).dirStructure));
			dirDateAndTimeInfoParseFromDirectoryEntry(str, &((*findEntryBuffer).dirStructure));
			
			(*findEntryBuffer).entryInfo.entryNumberOrOffset=str-(*searchingSecterBuffer).secterData.data;
			(*findEntryBuffer).entryInfo.location.clustor=(*searchingSecterBuffer).locatedClustor;
			(*findEntryBuffer).entryInfo.location.secterInClustor=(*searchingSecterBuffer).secterInClustor;

			
			sendDirEntryInfoToUsart0(findEntryBuffer);

			
			//reset findEntry info start//
			*((*findEntryBuffer).dirStructure.dirName.simple)=0;
			*((*findEntryBuffer).dirStructure.dirName.extension)=0;
			*((*findEntryBuffer).dirStructure.dirName.fullName)=0;
			(*findEntryBuffer).dirStructure.otherInfo.attribute=0;
			(*findEntryBuffer).entryInfo.extensionNameEntryCount=0;
			//reset findEntry info end//		
		}
		(*searchingSecterBuffer).secterInClustor++;
	}
	while( ((*searchingSecterBuffer).nextClustor != CLUSTOR_IS_END) || ((*searchingSecterBuffer).secterInClustor<((*diskInfo).secterPerClustor)) );
	(*findEntryBuffer).entryInfo.location.secterInClustor=(*diskInfo).secterPerClustor-1;

	return -1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char cmdDisplayArchive(fat32Info *diskInfo, clustorData *searchingSecterBuffer, directoryAndFileEntryInformation *foundFileEntryInfo)
{
	if((*foundFileEntryInfo).dirStructure.otherInfo.indicateFirstClustor==0) return -1;
	if((*foundFileEntryInfo).dirStructure.otherInfo.fileSize==0) return -2;
	if(((*foundFileEntryInfo).dirStructure.otherInfo.attribute&ATTR_ARCHIVE)!=ATTR_ARCHIVE) return -3;
	
	char *str;
	unsigned long readFileSize=0;
	unsigned long maximumFileSize=(*foundFileEntryInfo).dirStructure.otherInfo.fileSize;
	(*searchingSecterBuffer).locatedClustor=(*foundFileEntryInfo).dirStructure.otherInfo.indicateFirstClustor;
	(*searchingSecterBuffer).secterInClustor=0;


	checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);//if want check wrong fat table, added exception process

				sendString("----- FILE INFORMATION -----");
				sendDirectoryAndFileEntryInfomation1(foundFileEntryInfo);
				sendDirectoryAndFileEntryInfomation2(foundFileEntryInfo);
				sendDirectoryAndFileEntryInfomation3(foundFileEntryInfo);
				sendString("----- FILE VEIW IS START -----");
	
	do
	{
		if( !((*searchingSecterBuffer).secterInClustor < (*diskInfo).secterPerClustor) )//finding secter is lastest secter of clustor?
		{//lastest secter of clustor. loading next clustor
			(*searchingSecterBuffer).locatedClustor=(*searchingSecterBuffer).nextClustor;
			checkFatAndLocatNextClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor);
			(*searchingSecterBuffer).secterInClustor=0;
		}		


		readSecterInClustor(diskInfo, searchingSecterBuffer, (*searchingSecterBuffer).locatedClustor, (*searchingSecterBuffer).secterInClustor);

		for(str=(*searchingSecterBuffer).secterData.data; str<(*searchingSecterBuffer).secterData.data+SD_DATA_BUFFER_SIZE; str++)
		{
			if(readFileSize==maximumFileSize)
			{
				sendString("----- FILE VEIW IS END -----");
				return 0;
			}
			
			sendCharOnly((*str));
			readFileSize++;
		}
		(*searchingSecterBuffer).secterInClustor++;
	}
	while( ((*searchingSecterBuffer).nextClustor != CLUSTOR_IS_END) || ((*searchingSecterBuffer).secterInClustor<((*diskInfo).secterPerClustor)) );

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendDirectoryAndFileEntryInfomation1(directoryAndFileEntryInformation *dirEntryInfo)
{
		// sprintf(g_strBuf.dat, "S.N:%002x|%002x|%002x|%002x|%002x|%002x|%002x|%002x, E:%002x|%002x|%002x", *((*dirEntryInfo).dirStructure.dirName.simple+0), *((*dirEntryInfo).dirStructure.dirName.simple+1), *((*dirEntryInfo).dirStructure.dirName.simple+2), *((*dirEntryInfo).dirStructure.dirName.simple+3), *((*dirEntryInfo).dirStructure.dirName.simple+4), *((*dirEntryInfo).dirStructure.dirName.simple+5), *((*dirEntryInfo).dirStructure.dirName.simple+6), *((*dirEntryInfo).dirStructure.dirName.simple+7), *((*dirEntryInfo).dirStructure.dirName.extension+0), *((*dirEntryInfo).dirStructure.dirName.extension+1), *((*dirEntryInfo).dirStructure.dirName.extension+2));
		// sendString(g_strBuf.dat);

		// sprintf(g_strBuf.dat, "S.N:%c|%c|%c|%c|%c|%c|%c|%c, E:%c|%c|%c", *((*dirEntryInfo).dirStructure.dirName.simple+0), *((*dirEntryInfo).dirStructure.dirName.simple+1), *((*dirEntryInfo).dirStructure.dirName.simple+2), *((*dirEntryInfo).dirStructure.dirName.simple+3), *((*dirEntryInfo).dirStructure.dirName.simple+4), *((*dirEntryInfo).dirStructure.dirName.simple+5), *((*dirEntryInfo).dirStructure.dirName.simple+6), *((*dirEntryInfo).dirStructure.dirName.simple+7), *((*dirEntryInfo).dirStructure.dirName.extension+0), *((*dirEntryInfo).dirStructure.dirName.extension+1), *((*dirEntryInfo).dirStructure.dirName.extension+2));
		// sendString(g_strBuf.dat);
		
		sprintf(g_strBuf.dat, "S.N:%s, E:%s", (*dirEntryInfo).dirStructure.dirName.simple, (*dirEntryInfo).dirStructure.dirName.extension);
		sendString(g_strBuf.dat);

		sprintf(g_strBuf.dat, "LongName:%s",(*dirEntryInfo).dirStructure.dirName.fullName);
		sendString(g_strBuf.dat);

		sprintf(g_strBuf.dat, "CheckSumS:0x%002x, LongNameEntryNum:%002d", (*dirEntryInfo).entryInfo.extensionNameChkSum, (*dirEntryInfo).entryInfo.extensionNameEntryCount);
		sendString(g_strBuf.dat);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendDirectoryAndFileEntryInfomation2(directoryAndFileEntryInformation *dirEntryInfo)
{		sprintf(g_strBuf.dat, "simple Lo: clustor:0x%000000008lx, secter 0d%002d, entryOffset:0x%002x", (*dirEntryInfo).entryInfo.location.clustor,  (*dirEntryInfo).entryInfo.location.secterInClustor, (*dirEntryInfo).entryInfo.entryNumberOrOffset);
		sendString(g_strBuf.dat);

		sprintf(g_strBuf.dat, "size 0x%000000008lx, attribute 0x%002x, entry indicate:0x%000000008lx", (*dirEntryInfo).dirStructure.otherInfo.fileSize, (*dirEntryInfo).dirStructure.otherInfo.attribute, (*dirEntryInfo).dirStructure.otherInfo.indicateFirstClustor);
		sendString(g_strBuf.dat);
		
		//sprintf(buffer ,"empty C.L 0x%000000008lx", findEmptyClustor(&sdCardInfo, &clustor, (*dirEntryInfo).dirStructure.otherInfo.indicateFirstClustor));
		//putStringInGlcdAtPage(PAGE7, buffer);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendDirectoryAndFileEntryInfomation3(directoryAndFileEntryInformation *dirEntryInfo)
{
		sprintf(g_strBuf.dat, "Long Loc: Clustor:0x%000000008lx, secter:0d%002d, entryOffset:0x%002x", (*dirEntryInfo).entryInfo.longNameLocation.clustor, (*dirEntryInfo).entryInfo.longNameLocation.secterInClustor, (*dirEntryInfo).entryInfo.longNameEntryOffset);
		sendString(g_strBuf.dat);

		//sprintf(buffer ,"empty C.L 0x%000000008lx", findEmptyClustor(&sdCardInfo, &clustor, (*dirEntryInfo).dirStructure.otherInfo.indicateFirstClustor));
		//putStringInGlcdAtPage(PAGE7, buffer);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CMD_BUFFER_SIZE	257
char cmdStringNowLocationName[27]={0};
char cmd[CMD_BUFFER_SIZE]={0};

char *cmdArgs[MAXIMUM_CMD_ARGS_NUM]={0};
//char mkdirDirectoryBuffer[CMD_BUFFER_SIZE]={0};


CLUSTOR_LOCATION cmdClustorLocation;
char cmdResult=0;

int main()
{
	signed char resultBuffer = 0;

	CLUSTOR_LOCATION targetClustor;
	char fileName[31]={0};
	
/* GLCD INIT start */
	initGlcd();
	resetGlcd();
	_delay_ms(1);
/* GLCD INIT end */

	initUsart0();
	sendString("Usart Initializing...");

	displayDs1302ReadTime();
	nextSequence();

	SPI_Master_Init();
	sendString("SPI Master Initializing...");

	SPI_SD_CARD_Init(SPI_MODE_SD_CARD);
	sendString("SD Card Initializing...");

	findBootSecter(&sdCardInfo, &(clustor.secterData));



	strcpy(cmdStringNowLocationName, "/");
	cmdClustorLocation=sdCardInfo.rootClustor;

	unsigned char i=0;
	while(1)
	{

		sendStringOnly("Path:");
		sendString(cmdStringNowLocationName);
		sprintf(g_strBuf.dat, "Clus:0x%lx", cmdClustorLocation);
		sendString(g_strBuf.dat);
		sendStringOnly("$ ");

		receiveString(cmd);
		putStringInGlcdAtPage(PAGE2, cmd);
		sendStringOnly(cmd);

		cmdArgsLocation(cmdArgs, cmd);
		
		for(i=0; i<MAXIMUM_CMD_ARGS_NUM; i++)
		{
			sprintf(g_strBuf.dat,"args[%d]:", i);
			sendStringOnly(g_strBuf.dat);
			sendStringOnly((*(cmdArgs+i)));
			sendCharOnly(' ');
			
		}
		sendCharOnly('\r');
		sendCharOnly('\n');
		
		if(!strcmp(*(cmdArgs+0), "ls"))
		{
			cmdDisplayDirList(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation);
		}
		else if(!strcmp(*(cmdArgs+0), "cd"))
		{
			if(strlen(*(cmdArgs+1))!=0)
			{
				if((cmdResult=findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation, *(cmdArgs+1))))
				{
					sprintf(g_strBuf.dat, "%d:", cmdResult);
					sendStringOnly(g_strBuf.dat);
					sendString("directory is not found!");
					// sendDirectoryAndFileEntryInfomation1(&(fileBrowserData.findEntry));
					// sendDirectoryAndFileEntryInfomation2(&(fileBrowserData.findEntry));
					// sendDirectoryAndFileEntryInfomation3(&(fileBrowserData.findEntry));

				}
				else
				{
											// sendString(" - found directory info - ");
											// sendDirectoryAndFileEntryInfomation1(&(fileBrowserData.findEntry));
											// sendDirectoryAndFileEntryInfomation2(&(fileBrowserData.findEntry));
											// sendDirectoryAndFileEntryInfomation3(&(fileBrowserData.findEntry));
											// sprintf(g_strBuf.dat, "BeBefClus:0x%lx, %lx", cmdClustorLocation, (fileBrowserData.findEntry).dirStructure.otherInfo.indicateFirstClustor);
											// sendString(g_strBuf.dat);
					// if(!strcmp(*(cmdArgs+1), "."))
					// {
						// if((fileBrowserData.findEntry).dirStructure.otherInfo.indicateFirstClustor)
						// {
							// cmdClustorLocation=(fileBrowserData.findEntry).dirStructure.otherInfo.indicateFirstClustor;
							// strcpy(cmdStringNowLocationName, *(cmdArgs+1));
						// }
						// else
						// {
							// cmdClustorLocation=(sdCardInfo).rootClustor;
							// strcpy(cmdStringNowLocationName, "/");
						// }
					// }
					// else if(!strcmp(*(cmdArgs+1), ".."))
					// {
						// if((fileBrowserData.findEntry).dirStructure.otherInfo.indicateFirstClustor)
						// {
							// cmdClustorLocation=(fileBrowserData.findEntry).dirStructure.otherInfo.indicateFirstClustor;
							// strcpy(cmdStringNowLocationName, *(cmdArgs+1));
						// }
						// else
						// {
							// cmdClustorLocation=(sdCardInfo).rootClustor;
							// strcpy(cmdStringNowLocationName, "/");
						// }
					// }
					// else if(((fileBrowserData.findEntry).dirStructure.otherInfo.attribute&ATTR_DIRECTORY)!=ATTR_DIRECTORY)
					if(((fileBrowserData.findEntry).dirStructure.otherInfo.attribute&ATTR_DIRECTORY)!=ATTR_DIRECTORY)
					{
						sendString("target is not directory!");
					}
					else
					{
						if((fileBrowserData.findEntry).dirStructure.otherInfo.indicateFirstClustor!=0)
						{
													// sprintf(g_strBuf.dat, "BefClus:0x%lx, %lx", cmdClustorLocation, (fileBrowserData.findEntry).dirStructure.otherInfo.indicateFirstClustor);
													// putStringInGlcdAtPage(PAGE3, g_strBuf.dat);
													// sendString(g_strBuf.dat);

							cmdClustorLocation=(fileBrowserData.findEntry).dirStructure.otherInfo.indicateFirstClustor;

													// sprintf(g_strBuf.dat, "AftClus:0x%lx, %lx", cmdClustorLocation, (fileBrowserData.findEntry).dirStructure.otherInfo.indicateFirstClustor);
													// putStringInGlcdAtPage(PAGE4, g_strBuf.dat);
													// sendString(g_strBuf.dat);
							strcpy(cmdStringNowLocationName, *(cmdArgs+1));
						}
						else
						{
							cmdClustorLocation=(sdCardInfo).rootClustor;
							strcpy(cmdStringNowLocationName, "/");
						}
					}
				}
			}
			else
			{
				sendString("directory name is not inserted!");
			}
		}
		else if(!strcmp(*(cmdArgs+0), "mkdir"))
		{
			if(*(*(cmdArgs+1))=='/')//mkdir from root directory
			{
				sendString("absolute path is not support yet!");				
			}
			else
			{
				// memset(mkdirDirectoryBuffer, 0, CMD_BUFFER_SIZE);
				// strcpy(mkdirDirectoryBuffer, *(cmdArgs+1));
				// if(findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation, *(cmdArgs+1)))
				if((cmdResult=createNewDirEntry(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation, ATTR_DIRECTORY, *(cmdArgs+1))))
				// if(createNewDirEntry(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation, ATTR_DIRECTORY, mkdirDirectoryBuffer))
				{
					sprintf(g_strBuf.dat, "%d:", cmdResult);
					sendStringOnly(g_strBuf.dat);

					sendString("directory name is already exist!");
				}
			}
		}
		else if(!strcmp(*(cmdArgs+0), "mkfile"))
		{
			if(*(*(cmdArgs+1))=='/')//mkdir from root directory
			{
				sendString("absolute path is not support yet!");				
			}
			else
			{
				// memset(mkdirDirectoryBuffer, 0, CMD_BUFFER_SIZE);
				// strcpy(mkdirDirectoryBuffer, *(cmdArgs+1));
				// if(findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation, *(cmdArgs+1)))
				if((cmdResult=createNewDirEntry(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation, ATTR_ARCHIVE, *(cmdArgs+1))))
				// if(createNewDirEntry(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation, ATTR_ARCHIVE, mkdirDirectoryBuffer))
				{
					sprintf(g_strBuf.dat, "%d:", cmdResult);
					sendStringOnly(g_strBuf.dat);

					sendString("directory name is already exist!");
				}
			}
		}
		else if(!strcmp(*(cmdArgs+0), "rm"))
		{
			if(*(*(cmdArgs+1))=='/')//mkdir from root directory
			{
				sendString("absolute path is not support yet!");				
			}
			else
			{
//				if(findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation, *(cmdArgs+1)))
				if((cmdResult=findDirEntryAndDeleteUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation,  *(cmdArgs+1))))
				{
					sprintf(g_strBuf.dat, "%d:", cmdResult);
					sendStringOnly(g_strBuf.dat);

					sendString("delete dir or file fail!");
				}
			}
		}
		else if(!strcmp(*(cmdArgs+0), "find"))
		{
				if((cmdResult=findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation, *(cmdArgs+1))))
				{
					sprintf(g_strBuf.dat, "%d:", cmdResult);
					sendStringOnly(g_strBuf.dat);

					sendStringOnly("cannot find ");
					sprintf(g_strBuf.dat, "=> %s", *(cmdArgs+1));
					sendString(g_strBuf.dat);
					
				}
				else
				{
					sendDirectoryAndFileEntryInfomation1(&(fileBrowserData.findEntry));
					sendDirectoryAndFileEntryInfomation2(&(fileBrowserData.findEntry));
					sendDirectoryAndFileEntryInfomation3(&(fileBrowserData.findEntry));
				}
		}
		else if(!strcmp(*(cmdArgs+0), "put"))
		{
			if((cmdResult=putPasteWritingInFileIfNotCreateNewDirEntry(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), &fileAccessInfo, cmdClustorLocation, *(cmdArgs+1), *(cmdArgs+2), EndOfTextFile)))
			{
				sprintf(g_strBuf.dat, "%d:", cmdResult);
				sendStringOnly(g_strBuf.dat);

				sendStringOnly("cannot file write ");
				sprintf(g_strBuf.dat, "=> %s", *(cmdArgs+1));
				sendString(g_strBuf.dat);
			}
			else
			{
				sendStringOnly("file write success.");
			}
		}
		else if(!strcmp(*(cmdArgs+0), "cat"))
		{
			if((cmdResult=findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation, *(cmdArgs+1))))
			{
				sprintf(g_strBuf.dat, "%d:", cmdResult);
				sendStringOnly(g_strBuf.dat);

				sendStringOnly("cannot find file ");
				sprintf(g_strBuf.dat, "=> %s", *(cmdArgs+1));
				sendString(g_strBuf.dat);
			}
			else
			{
				cmdDisplayArchive(&sdCardInfo, &clustor, &(fileBrowserData.findEntry));
			}
		}
		
		
		
		else if(!strcmp(*(cmdArgs+0), "adc"))
		{
			if((cmdResult=findDirEntryUsingName(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), cmdClustorLocation, *(cmdArgs+1))))
			{
				sprintf(g_strBuf.dat, "%d:", cmdResult);
				sendStringOnly(g_strBuf.dat);

				sendStringOnly("cannot find file ");
				sprintf(g_strBuf.dat, "=> %s", *(cmdArgs+1));
				sendString(g_strBuf.dat);
			}
			else if((fileBrowserData.findEntry).dirStructure.otherInfo.indicateFirstClustor==0)
			{
				sprintf(g_strBuf.dat, "%d:", cmdResult);
				sendStringOnly(g_strBuf.dat);

				sendStringOnly("file is not have indicate clustor ");
				sprintf(g_strBuf.dat, "=> %s", *(cmdArgs+1));
				sendString(g_strBuf.dat);
			}
			else
			{
				sendString("----- GET ADC#0, #1 VALUES START -----");
				/*get value from ADC #0*/
				SPI_ADC_Init();

				sprintf(g_strBuf.dat ,
				"%0000006d,ADC%d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d", 
				(0x00001FFF&rand()),(0), 
				getAdcValue(SPI_MODE_ADC0, 0, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC0, 1, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC0, 2, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC0, 3, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES),
				getAdcValue(SPI_MODE_ADC0, 4, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC0, 5, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC0, 6, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC0, 7, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES)
				);
				sendString(g_strBuf.dat);

				SPI_SD_CARD_Init(SPI_MODE_SD_CARD);
				putPasteWritingInFile(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), &fileAccessInfo, g_strBuf.dat, EndOfTextFile);


				/*get value from ADC #1*/
				SPI_ADC_Init();

				sprintf(g_strBuf.dat ,
				"%0000006d,ADC%d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d", 
				(0x00001FFF&rand()),(1), 
				getAdcValue(SPI_MODE_ADC1, 0, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC1, 1, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC1, 2, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC1, 3, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES),
				getAdcValue(SPI_MODE_ADC1, 4, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC1, 5, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC1, 6, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
				getAdcValue(SPI_MODE_ADC1, 7, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES)
				 );
				sendString(g_strBuf.dat);


				SPI_SD_CARD_Init(SPI_MODE_SD_CARD);
				putPasteWritingInFile(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), &fileAccessInfo, g_strBuf.dat, EndOfTextFile);

				sendString("----- GET ADC#0, #1 VALUES END -----");
			}
		}
		else if(!strcmp(*(cmdArgs+0), "exit")) break;
		else
		{
			sendString("command is not found.");		
			sendString("command is....");
			sendString("ls | mkdir | rm | mkfile | find | find-long| put");		
		}
	}
	

	/*Setting file name and directory entry*/
	targetClustor=sdCardInfo.rootClustor;
	strcpy(fileName, "dataLoggerData.txt");
	/*using findTargetFileDirectoryEntryUsingName start*/

	
////////////////////////////////////////////////////* get Adc Value example and write sd-card start *////////////////////////////////////////////////////

	if(findDirEntryIfNotCreateNewDirEntry(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), targetClustor,  ATTR_ARCHIVE, fileName)==0)
	{
		unsigned long i=0;
////////////////////////////////////////////////////* get Adc Value example and write sd-card start *////////////////////////////////////////////////////
		for(i=0; i<0x40; i++)
		{
			/*get value from ADC #0*/
			SPI_ADC_Init();

			sprintf(g_strBuf.dat ,
			"%0000006d,ADC%d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d", 
			(0x00001FFF&rand()),(0), 
			getAdcValue(SPI_MODE_ADC0, 0, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC0, 1, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC0, 2, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC0, 3, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES),
			getAdcValue(SPI_MODE_ADC0, 4, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC0, 5, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC0, 6, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC0, 7, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES)
			);

			SPI_SD_CARD_Init(SPI_MODE_SD_CARD);
			putPasteWritingInFile(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), &fileAccessInfo, g_strBuf.dat, EndOfTextFile);


			/*get value from ADC #1*/
			SPI_ADC_Init();

			sprintf(g_strBuf.dat ,
			"%0000006d,ADC%d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d,%00004d", 
			(0x00001FFF&rand()),(1), 
			getAdcValue(SPI_MODE_ADC1, 0, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC1, 1, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC1, 2, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC1, 3, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES),
			getAdcValue(SPI_MODE_ADC1, 4, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC1, 5, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC1, 6, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES), 
			getAdcValue(SPI_MODE_ADC1, 7, 10, ADC_DEFAULT_JUNK_SAMPLING_TIMES)
			 );

			SPI_SD_CARD_Init(SPI_MODE_SD_CARD);
			putPasteWritingInFile(&sdCardInfo, &clustor, &(fileBrowserData.findEntry), &fileAccessInfo, g_strBuf.dat, EndOfTextFile);
		}
	}
////////////////////////////////////////////////////* get Adc Value example and write sd-card end *////////////////////////////////////////////////////

////////////////////////* finding Dir Entry Using indicate first clustor end *////////////////////////
////////////////////////////////////////* browser test start */////////////////////////////////////////
	SPI_SD_CARD_Init(SPI_MODE_SD_CARD);
	fileBrowser(&sdCardInfo, &clustor, &fileBrowserData);
////////////////////////////////////////* browser test start */////////////////////////////////////////
	return 0;
}

