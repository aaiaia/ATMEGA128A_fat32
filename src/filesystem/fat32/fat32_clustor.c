#include <fat32_clustor.h>

char setFirstClustorFATInfo(fat32Info *fatInfo, clustorData *p, unsigned long targetClustor)//when load dir located clustor or file.
{
	if(targetClustor < 2)	return -1;

	(*p).firstClustorLocation = targetClustor;

	return 0;
}

char readSecterInClustor(fat32Info *fatInfo, clustorData *p, unsigned long targetClustor, unsigned char targetSecterInClustor)
{
	if( (targetClustor < 2) && !(targetClustor < (*fatInfo).secterPerClustor) && (targetSecterInClustor < (*fatInfo).secterPerClustor) ) return -1;

	(*p).locatedClustorStartPhysicalSecter = (*fatInfo).firstDataSectorInFirstDataCluster + ( (targetClustor - 2) * ((unsigned long)(*fatInfo).secterPerClustor) );
	(*p).locatedClustor = targetClustor;

	(*p).secterInClustor = targetSecterInClustor;

	readSdcard(READ_SINGLE_BLOCK, &((*p).secterData), (*p).locatedClustorStartPhysicalSecter+targetSecterInClustor, 512, 1);

	return 0;
}

char writeSecterInClustor(fat32Info *fatInfo, clustorData *p, unsigned long targetClustor, unsigned char targetSecterInClustor)
{
	if( (targetClustor < 2) && (targetSecterInClustor < (*fatInfo).secterPerClustor) )	return -1;

	(*p).locatedClustorStartPhysicalSecter = (*fatInfo).firstDataSectorInFirstDataCluster + ( (targetClustor - 2) * ((unsigned long)(*fatInfo).secterPerClustor) );
	(*p).locatedClustor = targetClustor;

	(*p).secterInClustor = targetSecterInClustor;

	writeSdcard(WRITE_BLOCK, &((*p).secterData), (*p).locatedClustorStartPhysicalSecter+targetSecterInClustor, 512, 1);
	return 0;
}

CLUSTOR_LOCATION writeNextClustor(fat32Info *fatInfo, clustorData *p, unsigned long targetClustor, unsigned long writeClustorValue)
{
	if( (writeClustorValue < 2) || (writeClustorValue > (CLUSTOR_IS_END-1)) )
	{
		return -1;
	}
	CLUSTOR_LOCATION confirmClustor;
	/*
		void parsing32BitsToLittleEndian(char *str ,unsigned long number);
	*/
	readSdcard(READ_SINGLE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[0]+(targetClustor/FAT_PER_SECTER)), 512, 1);//return value that is lastest crc.

	confirmClustor=abstractLittleEndianTo32Bits( ((*p).secterData).data + BYTE_PER_ONE_FAT_ENTRY*(targetClustor%FAT_PER_SECTER) );
	if(confirmClustor!=CLUSTOR_IS_END)
	{
		return -1;
	}
	
	parsing32BitsToLittleEndian(((*p).secterData).data + BYTE_PER_ONE_FAT_ENTRY*(targetClustor%FAT_PER_SECTER) ,writeClustorValue);
	
	writeSdcard(WRITE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[0]+(targetClustor/FAT_PER_SECTER)), 512, 1);
	writeSdcard(WRITE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[1]+(targetClustor/FAT_PER_SECTER)), 512, 1);

	
	readSdcard(READ_SINGLE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[0]+(writeClustorValue/FAT_PER_SECTER)), 512, 1);//return value that is lastest crc.

	parsing32BitsToLittleEndian(((*p).secterData).data + BYTE_PER_ONE_FAT_ENTRY*(writeClustorValue%FAT_PER_SECTER) ,CLUSTOR_IS_END);

	writeSdcard(WRITE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[0]+(writeClustorValue/FAT_PER_SECTER)), 512, 1);
	writeSdcard(WRITE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[1]+(writeClustorValue/FAT_PER_SECTER)), 512, 1);

	unsigned char i;
	memset((*p).secterData.data, 0x00, SD_DATA_BUFFER_SIZE);
	for(i=0; i<(*fatInfo).secterPerClustor; i++)//reset new clustor.
	{
		writeSecterInClustor(fatInfo, p, writeClustorValue, i);
	}
	
	return writeClustorValue;
}

CLUSTOR_LOCATION writeNewClustor(fat32Info *fatInfo, clustorData *p, CLUSTOR_LOCATION targetClustor)
{
	CLUSTOR_LOCATION confirmClustor;
	/*
		void parsing32BitsToLittleEndian(char *str ,unsigned long number);
	*/
	readSdcard(READ_SINGLE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[0]+(targetClustor/FAT_PER_SECTER)), 512, 1);//return value that is lastest crc.

	confirmClustor=abstractLittleEndianTo32Bits( ((*p).secterData).data + BYTE_PER_ONE_FAT_ENTRY*(targetClustor%FAT_PER_SECTER) );
	if(confirmClustor!=0)
	{
		return -1;
	}
	
	parsing32BitsToLittleEndian(((*p).secterData).data + BYTE_PER_ONE_FAT_ENTRY*(targetClustor%FAT_PER_SECTER) ,CLUSTOR_IS_END);

	writeSdcard(WRITE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[0]+(targetClustor/FAT_PER_SECTER)), 512, 1);
	writeSdcard(WRITE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[1]+(targetClustor/FAT_PER_SECTER)), 512, 1);

	
	unsigned char i;
	memset((*p).secterData.data, 0x00, SD_DATA_BUFFER_SIZE);
	for(i=0; i<(*fatInfo).secterPerClustor; i++)//reset new clustor.
	{
		writeSecterInClustor(fatInfo, p, targetClustor, i);
	}

	return targetClustor;
}
char checkFatAndLocatNextClustor(fat32Info *fatInfo, clustorData *p, CLUSTOR_LOCATION targetClustor)
{
	readSdcard(READ_SINGLE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[0]+(targetClustor/FAT_PER_SECTER)), 512, 1);//return value that is lastest crc.
	(*p).nextClustor = abstractLittleEndianTo32Bits( ((*p).secterData).data + BYTE_PER_ONE_FAT_ENTRY*(targetClustor%FAT_PER_SECTER) );

	readSdcard(READ_SINGLE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[1]+(targetClustor/FAT_PER_SECTER)), 512, 1);//return value that is lastest crc.

	if((*p).nextClustor != abstractLittleEndianTo32Bits( ((*p).secterData).data + BYTE_PER_ONE_FAT_ENTRY*(targetClustor%FAT_PER_SECTER) ) )
	{
		return -1;
	}
	return 0;
}

CLUSTOR_LOCATION deleteClustor(fat32Info *fatInfo, clustorData *p, CLUSTOR_LOCATION targetClustor)
{
////////////////////////*delete Clustor start*////////////////////////
	// memset(findDirEntry, 0x00, sizeof(directoryAndFileEntryInformation));	
	// resultBuffer=254;

	// setDirBasicInfomation(&(*findDirEntry).dirStructure, "deleteFileTest.pdf", 0, 0);
	// setDirLongNameEntryInfomation(findDirEntry);
	// setTargetLocation(&(*findDirEntry).entryInfo.location, sdCardInfo.rootClustor, 0);
	// resultBuffer=findDirEntryUsingSimpleName(&sdCardInfo, &clustor, findDirEntry);
	
	// deleteClustor(&sdCardInfo, &clustor, (*findDirEntry).dirStructure.otherInfo.indicateFirstClustor);
////////////////////////*delete Clustor end*////////////////////////
	if(targetClustor == (*fatInfo).rootClustor)
	{
		return -1;
	}
	CLUSTOR_LOCATION locatedPhysicalSecter;
	CLUSTOR_LOCATION nextClustor;
	/*
		void parsing32BitsToLittleEndian(char *str ,unsigned long number);
	*/

	do
	{
		locatedPhysicalSecter	=	((*fatInfo).FATPhysicalSector[0]+(targetClustor/FAT_PER_SECTER));
		readSdcard(READ_SINGLE_BLOCK, &((*p).secterData), ((*fatInfo).FATPhysicalSector[0]+(targetClustor/FAT_PER_SECTER)), 512, 1);//return value that is lastest crc.
		do
		{
			nextClustor	=	abstractLittleEndianTo32Bits( ((*p).secterData).data + BYTE_PER_ONE_FAT_ENTRY*(targetClustor%FAT_PER_SECTER) );
			parsing32BitsToLittleEndian(((*p).secterData).data + BYTE_PER_ONE_FAT_ENTRY*(targetClustor%FAT_PER_SECTER) ,0);
			targetClustor=nextClustor;
		}
		while( (locatedPhysicalSecter == ((*fatInfo).FATPhysicalSector[0]+(targetClustor/FAT_PER_SECTER))) && (targetClustor != 0)  &&(targetClustor != CLUSTOR_IS_END) );

		writeSdcard(WRITE_BLOCK, &((*p).secterData), locatedPhysicalSecter, 512, 1);
		writeSdcard(WRITE_BLOCK, &((*p).secterData), locatedPhysicalSecter+((*fatInfo).FATPhysicalSector[1]-(*fatInfo).FATPhysicalSector[0]), 512, 1);
	}
	while((targetClustor!=CLUSTOR_IS_END)&&(targetClustor!=0)&&(targetClustor!=1));

	return targetClustor;
}

char readClostor(fat32Info *fatInfo, clustorData *p, unsigned long targetClustor)
{
	if( (targetClustor < 2) && !(targetClustor < (*fatInfo).secterPerClustor)) return -1;//before read clustor data, clustor infomation is had to load.

	(*p).locatedClustor = targetClustor;
	(*p).locatedClustorStartPhysicalSecter = (*fatInfo).firstDataSectorInFirstDataCluster + ( (targetClustor - 2) * ((unsigned long)(*fatInfo).secterPerClustor) );

	(*p).secterInClustor = (*fatInfo).secterPerClustor;//target clustor, if same to secter per clustor read clustor

	
	if( checkFatAndLocatNextClustor(fatInfo, p, targetClustor) ) return -1;


	unsigned int secterCount;

	for(secterCount=0; secterCount<(*fatInfo).secterPerClustor; secterCount++)
	{
		readSdcard(READ_SINGLE_BLOCK, &((*p).secterData), (*p).locatedClustorStartPhysicalSecter+secterCount, 512, 1);
	}
	return 0;
}

CLUSTOR_LOCATION findEmptyClustor(fat32Info *fatInfo, clustorData *buffer, unsigned long startClustor)
{
	////////////////////////////*emtpy clustor trace code start*////////////////////////////
	/* If find empty clustor successfully, findEmptyClustor return clustor number. But, if not find empry clustor, it return 0. */
	/*

			(testClustor = findEmptyClustor(&sdCardInfo, &clustor, testClustor));
			sprintf(buffer, "empty clustor %5d", testClustor);
			putStringInGlcdAtPage(PAGE1, buffer);
			(testClustor = findEmptyClustor(&sdCardInfo, &clustor, testClustor+1));
			sprintf(buffer, "empty clustor %5d", testClustor);
			putStringInGlcdAtPage(PAGE2, buffer);
			(testClustor = findEmptyClustor(&sdCardInfo, &clustor, testClustor+1));
			sprintf(buffer, "empty clustor %5d", testClustor);
			putStringInGlcdAtPage(PAGE3, buffer);
			(testClustor = findEmptyClustor(&sdCardInfo, &clustor, testClustor+1));
			sprintf(buffer, "empty clustor %5d", testClustor);
			putStringInGlcdAtPage(PAGE4, buffer);
			(testClustor = findEmptyClustor(&sdCardInfo, &clustor, testClustor+1));
			sprintf(buffer, "empty clustor %5d", testClustor);
			putStringInGlcdAtPage(PAGE5, buffer);
			(testClustor = findEmptyClustor(&sdCardInfo, &clustor, testClustor+1));
			sprintf(buffer, "empty clustor %5d", testClustor);
			putStringInGlcdAtPage(PAGE6, buffer);
			(testClustor = findEmptyClustor(&sdCardInfo, &clustor, testClustor+1));
			sprintf(buffer, "empty clustor %5d", testClustor);
			putStringInGlcdAtPage(PAGE7, buffer);
			(testClustor = findEmptyClustor(&sdCardInfo, &clustor, testClustor+1));
			sprintf(buffer, "empty clustor %5d", testClustor);
			putStringInGlcdAtPage(PAGE8, buffer);
																						nextSequence();
			resetGlcd();
			putStringInGlcdAtPage(PAGE1, "test find lastest clustor");
			sprintf(buffer, "0x%000000008lx had lastest", testClustor);
			putStringInGlcdAtPage(PAGE2, buffer);
			putStringInGlcdAtPage(PAGE3, "clustor is...");
			sprintf(buffer, "0x%000000008lx", findFilesLastestClustor(&sdCardInfo, &clustor, testClustor));
			putStringInGlcdAtPage(PAGE4, buffer);
																						nextSequence();
	*/
	////////////////////////////*emtpy clustor trace code end*////////////////////////////
	CLUSTOR_LOCATION clustorNumber = startClustor;
	unsigned char i;
	unsigned char zeroCounter;

	char *searchingFat;

	zeroCounter = 0;
	searchingFat = ((*buffer).secterData).data+(BYTE_PER_ONE_FAT_ENTRY*(startClustor%FAT_PER_SECTER));

	//total sector number(clustorNumber < ((*fatInfo).FATPhysicalSector[1]-(*fatInfo).FATPhysicalSector[0])*FAT_PER_SECTER
	while((zeroCounter < BYTE_PER_ONE_FAT_ENTRY) && (clustorNumber < ( (*fatInfo).FATPhysicalSector[1] - (*fatInfo).FATPhysicalSector[0] )*FAT_PER_SECTER) )//(FATPhysicalSector[1]-FATPhysicalSector[0])/FAT_PER_SECTER->one fat table have fats
	{
		readSdcard(READ_SINGLE_BLOCK, &((*buffer).secterData), ((*fatInfo).FATPhysicalSector[0]+(clustorNumber/FAT_PER_SECTER)), 512, 1);
		while(searchingFat<((*buffer).secterData).data+SD_DATA_BUFFER_SIZE)
		{
			for(i=0; i<BYTE_PER_ONE_FAT_ENTRY; i++)
			{
				if(*(searchingFat+i) == 0)
				{
					zeroCounter++;
				}
			}
			if(!(zeroCounter < BYTE_PER_ONE_FAT_ENTRY)) break;

			searchingFat+=i;
			clustorNumber++;
			zeroCounter = 0;
		}
		searchingFat = ((*buffer).secterData).data;	
	}

	if(zeroCounter == BYTE_PER_ONE_FAT_ENTRY)
	{
		return clustorNumber;
	}


	clustorNumber = 2;
	zeroCounter = 0;

	while((zeroCounter < BYTE_PER_ONE_FAT_ENTRY) && (clustorNumber < startClustor))//(FATPhysicalSector[1]-FATPhysicalSector[0])/FAT_PER_SECTER->one fat table have fats
	{
		readSdcard(READ_SINGLE_BLOCK, &((*buffer).secterData), ((*fatInfo).FATPhysicalSector[0]+(clustorNumber/FAT_PER_SECTER)), 512, 1);

		while(searchingFat<((*buffer).secterData).data+SD_DATA_BUFFER_SIZE)
		{
			for(i=0; i<BYTE_PER_ONE_FAT_ENTRY; i++)
			{
				if(*(searchingFat+i) == 0)
				{
					zeroCounter++;
				}
			}
			if(zeroCounter == BYTE_PER_ONE_FAT_ENTRY) break;

			searchingFat+=i;
			clustorNumber++;
			zeroCounter = 0;
		}
		searchingFat = ((*buffer).secterData).data;	
	}

	if(zeroCounter == BYTE_PER_ONE_FAT_ENTRY)
	{
		return clustorNumber;
	}
	else
	{
		return 0;
	}
}

CLUSTOR_LOCATION findFilesLastestClustor(fat32Info *fatInfo, clustorData *buffer, CLUSTOR_LOCATION clustorNumber)//return number of sum of clustor - 1
{
	/*findFilesLastestClustor test code start */
	/*
			sprintf(buffer, "0x%000000008lx had lastest", testClustor);
			putStringInGlcdAtPage(PAGE2, buffer);
			putStringInGlcdAtPage(PAGE3, "clustor is...");
			sprintf(buffer, "0x%000000008lx", findFilesLastestClustor(&sdCardInfo, &clustor, testClustor));
			putStringInGlcdAtPage(PAGE4, buffer);
																						nextSequence();
	*/
	/*findFilesLastestClustor test code end */
	if(!clustorNumber)	return clustorNumber;

	CLUSTOR_LOCATION clustorNumberBuffer = clustorNumber;

	char *searchingFat;


	readSdcard(READ_SINGLE_BLOCK, &((*buffer).secterData), ((*fatInfo).FATPhysicalSector[0]+(clustorNumber/FAT_PER_SECTER)), 512, 1);

	searchingFat = ((*buffer).secterData).data+(BYTE_PER_ONE_FAT_ENTRY*(clustorNumber%FAT_PER_SECTER));
	clustorNumber = abstractLittleEndianTo32Bits(searchingFat);
	while( clustorNumber!=CLUSTOR_IS_END )
	{
		clustorNumberBuffer = clustorNumber;
		if( ((*fatInfo).FATPhysicalSector[0]+(clustorNumber/FAT_PER_SECTER)) !=  (*buffer).secterData.physicalSectorNumber)
		{
			readSdcard(READ_SINGLE_BLOCK, &((*buffer).secterData), ((*fatInfo).FATPhysicalSector[0]+(clustorNumber/FAT_PER_SECTER)), 512, 1);
		}
		clustorNumber = abstractLittleEndianTo32Bits(searchingFat);

		searchingFat = ((*buffer).secterData).data+(BYTE_PER_ONE_FAT_ENTRY*(clustorNumber%FAT_PER_SECTER));
	}
	
	return clustorNumberBuffer;
}

CLUSTOR_LOCATION findBeforeClustor(fat32Info *fatInfo, clustorData *buffer, CLUSTOR_LOCATION searchingClustorNumber, CLUSTOR_LOCATION refferedClustorNumber)//return number of sum of clustor - 1
{
	if(searchingClustorNumber == 0)	return searchingClustorNumber;
	else if(refferedClustorNumber == searchingClustorNumber) return searchingClustorNumber;
	
	CLUSTOR_LOCATION clustorNumberBuffer = searchingClustorNumber;

	char *searchingFat;


	readSdcard(READ_SINGLE_BLOCK, &((*buffer).secterData), ((*fatInfo).FATPhysicalSector[0]+(searchingClustorNumber/FAT_PER_SECTER)), 512, 1);

	searchingFat = ((*buffer).secterData).data+(BYTE_PER_ONE_FAT_ENTRY*(searchingClustorNumber%FAT_PER_SECTER));
	searchingClustorNumber = abstractLittleEndianTo32Bits(searchingFat);
	while( searchingClustorNumber!=refferedClustorNumber )
	{
		clustorNumberBuffer = searchingClustorNumber;
		if( ((*fatInfo).FATPhysicalSector[0]+(searchingClustorNumber/FAT_PER_SECTER)) !=  (*buffer).secterData.physicalSectorNumber)
		{
			readSdcard(READ_SINGLE_BLOCK, &((*buffer).secterData), ((*fatInfo).FATPhysicalSector[0]+(searchingClustorNumber/FAT_PER_SECTER)), 512, 1);
		}
		searchingClustorNumber = abstractLittleEndianTo32Bits(searchingFat);

		searchingFat = ((*buffer).secterData).data+(BYTE_PER_ONE_FAT_ENTRY*(searchingClustorNumber%FAT_PER_SECTER));
	}
	
	return clustorNumberBuffer;
}

CLUSTOR_LOCATION findNthClustor(fat32Info *fatInfo, clustorData *buffer, CLUSTOR_LOCATION searchingClustorNumber, unsigned long clustorOffset)//return number of sum of clustor - 1
{
	if(searchingClustorNumber == 0)	return 0;
	else if(clustorOffset == 0) return searchingClustorNumber;
	
	CLUSTOR_LOCATION clustorNumberBuffer = searchingClustorNumber;

	char *searchingFat;


	readSdcard(READ_SINGLE_BLOCK, &((*buffer).secterData), ((*fatInfo).FATPhysicalSector[0]+(searchingClustorNumber/FAT_PER_SECTER)), 512, 1);

	searchingFat = ((*buffer).secterData).data+(BYTE_PER_ONE_FAT_ENTRY*(searchingClustorNumber%FAT_PER_SECTER));
	searchingClustorNumber = abstractLittleEndianTo32Bits(searchingFat);//0th offset.
	while( (searchingClustorNumber!=CLUSTOR_IS_END) && (clustorOffset!=0) )
	{
		clustorNumberBuffer = searchingClustorNumber;
		if( ((*fatInfo).FATPhysicalSector[0]+(searchingClustorNumber/FAT_PER_SECTER)) !=  (*buffer).secterData.physicalSectorNumber)
		{
			readSdcard(READ_SINGLE_BLOCK, &((*buffer).secterData), ((*fatInfo).FATPhysicalSector[0]+(searchingClustorNumber/FAT_PER_SECTER)), 512, 1);
		}
		searchingClustorNumber = abstractLittleEndianTo32Bits(searchingFat);
		clustorOffset--;
		
		
		searchingFat = ((*buffer).secterData).data+(BYTE_PER_ONE_FAT_ENTRY*(searchingClustorNumber%FAT_PER_SECTER));
	}
	
	if(clustorOffset)
	{
		return clustorNumberBuffer;
	}
	else
	{
		return searchingClustorNumber;
	}
}

unsigned long findFilesLastestLocationInClustor(fat32Info *fatInfo, clustorData *buffer, unsigned long fileSize)
{
	/*findFilesLastestLocationInClustor test code start*/
	/*
		putStringInGlcdAtPage(PAGE5, "start clustor is...");
		sprintf(buffer, "0x%000000008lx", testClustor);
		putStringInGlcdAtPage(PAGE6, buffer);
		putStringInGlcdAtPage(PAGE7, "lastest data offset");
		sprintf(buffer, "0d%000000008ld", findFilesLastestLocationInClustor(&sdCardInfo, &clustor, findFilesLastestClustor(&sdCardInfo, &clustor, testClustor)));
		putStringInGlcdAtPage(PAGE8, buffer);
																					nextSequence();
	*/
	/*findFilesLastestLocationInClustor test code end*/
	unsigned long fileSizeIsMultipleOfBytesPerClustor=(unsigned long)(fileSize/(((unsigned long)((*fatInfo).bytesPerSecter))*((unsigned long)((*fatInfo).secterPerClustor))));
	unsigned long offsetInClustor=(unsigned long)(fileSize%(((unsigned long)((*fatInfo).bytesPerSecter))*((unsigned long)((*fatInfo).secterPerClustor))));
	
	if( (fileSizeIsMultipleOfBytesPerClustor!=0) && (offsetInClustor==0))
	{
		return ((unsigned long)((*fatInfo).bytesPerSecter))*((unsigned long)((*fatInfo).secterPerClustor));
	}
	else
	{
		return offsetInClustor;
	}
}
