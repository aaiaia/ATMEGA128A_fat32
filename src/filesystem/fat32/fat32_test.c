void displayFAT32Info(fat32Info *p)
{
	sprintf(g_glcdBuf ,"FileType:%s", (*p).fileSystemType);
	putStringInGlcdAtPage(PAGE1, g_glcdBuf);
	sprintf(g_glcdBuf ,"Bytes/Sector");
	putStringInGlcdAtPage(PAGE2, g_glcdBuf);
	sprintf(g_glcdBuf ,"0d%d 0x%x", (*p).bytesPerSecter, (*p).bytesPerSecter);
	putStringInGlcdAtPage(PAGE3, g_glcdBuf);
	sprintf(g_glcdBuf ,"Sector/Clustor");
	putStringInGlcdAtPage(PAGE4, g_glcdBuf);
	sprintf(g_glcdBuf ,"0d%d 0x%x", (*p).secterPerClustor, (*p).secterPerClustor);
	putStringInGlcdAtPage(PAGE5, g_glcdBuf);

	sprintf(g_glcdBuf ,"Reserved Sector");
	putStringInGlcdAtPage(PAGE6, g_glcdBuf);
	sprintf(g_glcdBuf ,"0d%d 0x%x", (*p).reservedSectorCount, (*p).reservedSectorCount);
	putStringInGlcdAtPage(PAGE7, g_glcdBuf);

	sprintf(g_glcdBuf ,"NumFats 0x%x", (*p).numberOfFATs);
	putStringInGlcdAtPage(PAGE8, g_glcdBuf);

	nextSequence();

	sprintf(g_glcdBuf ,"Hidden Sector");
	putStringInGlcdAtPage(PAGE1, g_glcdBuf);
	sprintf(g_glcdBuf ,"0d%ld 0x%lx", (*p).hiddenSector, (*p).hiddenSector);
	putStringInGlcdAtPage(PAGE2, g_glcdBuf);
	sprintf(g_glcdBuf ,"Total Sector");
	putStringInGlcdAtPage(PAGE3, g_glcdBuf);
	sprintf(g_glcdBuf ,"0d%ld", (*p).totalSector);
	putStringInGlcdAtPage(PAGE4, g_glcdBuf);
	sprintf(g_glcdBuf ,"0x%lx", (*p).totalSector);
	putStringInGlcdAtPage(PAGE5, g_glcdBuf);
	sprintf(g_glcdBuf ,"FAT occpied secter");
	putStringInGlcdAtPage(PAGE6, g_glcdBuf);
	sprintf(g_glcdBuf ,"0d%ld", (*p).FATSz32);
	putStringInGlcdAtPage(PAGE7, g_glcdBuf);
	sprintf(g_glcdBuf ,"0x%lx", (*p).FATSz32);
	putStringInGlcdAtPage(PAGE8, g_glcdBuf);

	nextSequence();

	resetGlcd();
	sprintf(g_glcdBuf ,"Root Clustor");
	putStringInGlcdAtPage(PAGE1, g_glcdBuf);
	sprintf(g_glcdBuf ,"0x%lx", (*p).rootClustor);
	putStringInGlcdAtPage(PAGE2, g_glcdBuf);

	sprintf(g_glcdBuf ,"FAT secter");
	putStringInGlcdAtPage(PAGE3, g_glcdBuf);
	sprintf(g_glcdBuf, "0x%lx 0x%lx", (*p).FATPhysicalSector[0], (*p).FATPhysicalSector[1]);
	putStringInGlcdAtPage(PAGE4, g_glcdBuf);

	sprintf(g_glcdBuf ,"Root directory");
	putStringInGlcdAtPage(PAGE5, g_glcdBuf);
	sprintf(g_glcdBuf ,"Sector Offset");
	putStringInGlcdAtPage(PAGE6, g_glcdBuf);
	sprintf(g_glcdBuf ,"0d%ld", (*p).rootDirectoryPhysicalSector);
	putStringInGlcdAtPage(PAGE7, g_glcdBuf);
	sprintf(g_glcdBuf ,"0x%lx", (*p).rootDirectoryPhysicalSector);
	putStringInGlcdAtPage(PAGE8, g_glcdBuf);
	nextSequence();
}
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
