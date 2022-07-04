#ifndef _FAT32_CLUSTOR_H_
#define _FAT32_CLUSTOR_H_

#define BYTE_PER_ONE_FAT_ENTRY					(sizeof(unsigned long))//4
#define FAT_PER_SECTER							(SD_DATA_BUFFER_SIZE/BYTE_PER_ONE_FAT_ENTRY)///need to change variables 512/4=128

#define CLUSTOR_LOCATION		unsigned long
#define	NEXT_CLUSTOR_LOCATION	unsigned long

#define	CLUSTOR_IS_END			0x0fffffff

struct clustorData
{
	SD_RW_Data				secterData;


	CLUSTOR_LOCATION		firstClustorLocation;//when input, directory or file in.


	CLUSTOR_LOCATION		locatedClustor;
	unsigned long			locatedClustorStartPhysicalSecter;
	unsigned char			secterInClustor;

	NEXT_CLUSTOR_LOCATION	nextClustor;
}typedef clustorData;

clustorData clustor;

#endif
