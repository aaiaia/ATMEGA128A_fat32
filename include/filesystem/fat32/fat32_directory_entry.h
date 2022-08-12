#ifndef _FAT32_DIRECTORY_H_
#define _FAT32_DIRECTORY_H_
/**//**//**//**//**//**//**//**//**//**//**/
/*Definition of Directory Structure start*/
/**//**//**//**//**//**//**//**//**//**//**/
#define DIR_DISCRIPTION_LENGTH	32//(bytes)

#define DIR_EMPTY				0x00
#define DIR_DELEDTED			0xE5

#define DIR_NAME_OFFSET			0
#define DIR_FILE_SIZE_OFFSET	28

#define DIR_SIMPLE_NAME_MAXIMUM_LENGTH	8
#define DIR_EXTENSION_MAXUMUM_LENGTH	3

#define DIR_LONG_NAME_MARKER_OFFSET	((DIR_SIMPLE_NAME_MAXIMUM_LENGTH-1)-1)//posion of '~' in simple Name. Because change name length to offset minus 1(-1).
#define DIR_LONG_NAME_MARKER		'~'
#define DIR_NAME_EMPTY_DATA		0x20

#define SIMPLE_NAME_OFFSET		DIR_NAME_OFFSET//8bytes
#define EXTENSION_OFFSET	(SIMPLE_NAME_OFFSET+DIR_SIMPLE_NAME_MAXIMUM_LENGTH)//3bytes

#define DIR_ATTR_OFFSET			11//(1bytes)

#define ATTR_READ_ONLY			0x01
#define ATTR_HIDDEN				0x02
#define ATTR_SYSTEM				0x04
#define ATTR_VOLUME_ID			0x08
#define ATTR_DIRECTORY			0x10
#define ATTR_ARCHIVE			0x20

#define ATTR_DIR_FILE_MASK		0xF0
#define ATTR_ACCESS_AUTHORITY_MASK	0x0F

#define MSB_FIRST_CLUSTOR_OFFSET	20
#define LSB_FIRST_CLUSTOR_OFFSET	26

#define DIR_FILESIZE				28


#define ATTRIBUTE_MASK			(ATTR_READ_ONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_VOLUME_ID)
/**//**//**//**//**//**//**//**//**//**//**/
/*Definition of Directory Structure end*/
/**//**//**//**//**//**//**//**//**//**//**/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Time definition of time start*/
#define DIR_CREATION_TIME_TENTH_OFFSET	13
#define DIR_CREATION_TIME_OFFSET		14
#define DIR_CREATION_DATE_OFFSET		16

#define DIR_LAST_ACCESS_DATE_OFFSET		18
#define DIR_WRITE_TIME_OFFSET			22
#define DIR_WRITE_DATE_OFFSET			24

/*Time definition of time end*/
/*Date and Time definition start*/
#define DIR_DATE_YEAR_START				1980

#define DIR_DATE_YEAR_BIT_OFFSET		9
#define DIR_DATE_MONTH_BIT_OFFSET		5
#define DIR_DATE_DAY_BIT_OFFSET			0

#define DIR_TIME_HOUR_BIT_OFFSET		11
#define DIR_TIME_MINUTE_BIT_OFFSET		5
#define DIR_TIME_SECOND_BIT_OFFSET		0

/*To abstract data info formed bits use a MASK.*/
#define DIR_DATE_YEAR_BIT_MASK			(0x7F<<DIR_DATE_YEAR_BIT_OFFSET)
#define DIR_DATE_MONTH_BIT_MASK			(0x0F<<DIR_DATE_MONTH_BIT_OFFSET)
#define DIR_DATE_DAY_BIT_MASK			(0x1F<< DIR_DATE_DAY_BIT_OFFSET)

#define DIR_TIME_HOUR_BIT_MASK			(0x1F<<DIR_TIME_HOUR_BIT_OFFSET)
#define DIR_TIME_MINUTE_BIT_MASK		(0x3F<<DIR_TIME_MINUTE_BIT_OFFSET)
#define DIR_TIME_SECOND_BIT_MASK		(0x1F<<DIR_TIME_SECOND_BIT_OFFSET)
/*Date and Time definition end*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**//**//**//**//**//**//**//**//**//**//**/
/*Long File Name Implementation Start*/
/**//**//**//**//**//**//**//**//**//**//**/
#define LONG_DIR_NAME_ONE_WORD_SIZE	0x02

#define LONG_DIR_NAME1_OFFSET	0x01
#define LONG_DIR_NAME1_SIZE		0x0A
#define LONG_DIR_NAME1_WORDS	(LONG_DIR_NAME1_SIZE/LONG_DIR_NAME_ONE_WORD_SIZE)

#define ATTR_LONG_NAME			ATTRIBUTE_MASK	
#define ATTR_LONG_NAME_MASK		(ATTR_READ_ONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_VOLUME_ID|ATTR_DIRECTORY|ATTR_ARCHIVE)
#define LONG_DIR_ATTRIBUTE_OFFSET	0x0B

#define LONG_DIR_TYPE_OFFSET		0x0C

#define LONG_DIR_CHECK_SUM_OFFSET	0x0D

#define LONG_DIR_NAME2_OFFSET	0x0E
#define LONG_DIR_NAME2_SIZE		0x0C
#define LONG_DIR_NAME2_WORDS	(LONG_DIR_NAME2_SIZE/LONG_DIR_NAME_ONE_WORD_SIZE)

#define LONG_DIR_FIRST_CLUSTOR_LOCATION	0x1A

#define LONG_DIR_NAME3_OFFSET	0x1C
#define LONG_DIR_NAME3_SIZE		0x04
#define LONG_DIR_NAME3_WORDS	(LONG_DIR_NAME3_SIZE/LONG_DIR_NAME_ONE_WORD_SIZE)

#define LONG_DIR_CHECK_SUM_OFFSET	13

///*MCU recognize long name length start*///
#define LONG_DIR_ORDER_OFFSET	0x00

#define LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY		13
#define LONG_NAME_ENTRY_MAXIMUM_NUMBER		0x02

#define LONG_NAME_LASTEST_MASK						0xF0//not used
#define LONG_NAME_NUMBER_MASK						0x0F
#define LONG_NAME_LASTEST_VALID_VALUE				0x40

#define LONG_NAME_MAXIMUM_LENGTH					(LONG_NAME_ENTRY_MAXIMUM_NUMBER*LONG_NAME_CHARACTER_NUMBER_IN_A_ENTRY)
///*MCU recognize long name length end*///
/**//**//**//**//**//**//**//**//**//**//**/
/*Long File Name Implementation End*/
/**//**//**//**//**//**//**//**//**//**//**/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct directoryName
{
	char simple[DIR_SIMPLE_NAME_MAXIMUM_LENGTH+1];
	char extension[DIR_EXTENSION_MAXUMUM_LENGTH+1];
	char fullName[LONG_NAME_MAXIMUM_LENGTH+1];
}typedef directoryName;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct dirDateInfo
{
	unsigned int year;
	unsigned char month;
	unsigned char day;
}typedef dirDateInfo;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct dirTimeInfo
{
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
}typedef dirTimeInfo;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct dirDateAndTimeInfo
{
	dirDateInfo date;
	dirTimeInfo time;
}typedef dirDateAndTimeInfo;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct dirOtherInfo
{
	unsigned char attribute;

	CLUSTOR_LOCATION indicateFirstClustor;

	unsigned long fileSize;
}typedef dirOtherInfo;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct directoryStructure
{
	directoryName dirName;

	dirOtherInfo otherInfo;

	dirDateAndTimeInfo createDateInfo;
	dirDateAndTimeInfo writeDateInfo;

//	dirDateAndTime lastestAccessDate;
	dirDateInfo lastestAccessDate;
}typedef directoryStructure;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
