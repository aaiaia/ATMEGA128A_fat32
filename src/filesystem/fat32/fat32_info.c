#include <fat32_info.h>

char abstractFat32Info(fat32Info *p, SD_RW_Data *target)
{
	memset(p, 0x00, sizeof(fat32Info));

	if((*target).data[0] != 0xEB)
	{
		memset(p, 0x00, sizeof(fat32Info));
		return 0xFF;
	}
	if((*target).data[2] != 0x90)
	{
		memset(p, 0x00, sizeof(fat32Info));
		return 0xFF;
	}

	(*p).bytesPerSecter = abstractLittleEndianTo16Bits((*target).data+11);

	(*p).secterPerClustor = (*target).data[13];

	(*p).reservedSectorCount = abstractLittleEndianTo16Bits((*target).data+14);

	(*p).numberOfFATs = (*target).data[16];

	(*p).hiddenSector = abstractLittleEndianTo32Bits((*target).data+28);

	(*p).totalSector = abstractLittleEndianTo32Bits((*target).data+32);

	(*p).FATSz32 = abstractLittleEndianTo32Bits((*target).data+36);

	(*p).rootClustor = abstractLittleEndianTo32Bits((*target).data+44);

	strncpy((*p).fileSystemType, (*target).data+82, 8);

	//FATPhysicalSector[4];
	if((*p).numberOfFATs == 0x02)
	{
		*((*p).FATPhysicalSector) = (*p).hiddenSector + (*p).reservedSectorCount;
		*((*p).FATPhysicalSector+1) = (*p).hiddenSector + ((*p).reservedSectorCount + (*p).FATSz32);
	}
	else
	{
		return 0xFF;
	}


	(*p).firstDataSectorInFirstDataCluster =
		(
			(*p).hiddenSector + 
			(*p).reservedSectorCount + 
			( (*p).FATSz32 * (*p).numberOfFATs )
		);


	(*p).rootDirectoryPhysicalSector = 
		(
			(*p).firstDataSectorInFirstDataCluster +
			( ( ( (*p).rootClustor) - 2 ) * (*p).secterPerClustor )
		);
	
	return 0;
}

void findBootSecter(fat32Info *diskInfo, SD_RW_Data *target)
{
/*Abstract Fat32 Information start*/
	// find8bytes(MBR_KEY_WORD, &(clustor.secterData), 0);
	// abstractFat32Info(&sdCardInfo, &(clustor.secterData));
/*Abstract Fat32 Information end*/

/*
	example code
	findBootSecter(&sdCardInfo, &(clustor.secterData));
*/

	find8bytes(MBR_KEY_WORD, target, 0);
	abstractFat32Info(diskInfo, target);
}

