#ifndef _FAT32_INFO_H_
#define _FAT32_INFO_H_
/*FAT32*/
struct fat32Info
{
	unsigned int bytesPerSecter;
	unsigned char secterPerClustor;

	unsigned int reservedSectorCount;

	unsigned char numberOfFATs;
	unsigned long hiddenSector;
	unsigned long totalSector;
	unsigned long FATSz32;
	unsigned long rootClustor;
	char fileSystemType[9];
	unsigned long FATPhysicalSector[2];
	unsigned long firstDataSectorInFirstDataCluster;
	unsigned long rootDirectoryPhysicalSector;
}typedef fat32Info;

const char MBR_KEY_WORD[9] = {0xEB, 0x58, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x00};//

fat32Info sdCardInfo;

#endif
