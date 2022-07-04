unsigned long abstractLittleEndianTo32Bits(char *p)
{
	unsigned long result = 0;
	result=((unsigned long)p[0]);
	result|=(((unsigned long)p[1])<<8);
	result|=(((unsigned long)p[2])<<16);
	result|=(((unsigned long)p[3])<<24);

	return result;
}

unsigned int abstractLittleEndianTo16Bits(char *p)
{
	unsigned int result = 0;
	result=((unsigned int)p[0]);
	result|=(((unsigned int)p[1])<<8);

	return result;
}

void parsing16BitsToLittleEndian(char *str ,unsigned int number)
{
	*(str)=number;
	*(str+1)=(number>>8);
}

void parsing32BitsToLittleEndian(char *str ,unsigned long number)
{
	*(str)=number;
	number=(number>>8);
	*(str+1)=number;
	number=(number>>8);
	*(str+2)=number;
	number=(number>>8);
	*(str+3)=number;
}

char addSdCardBuffer(char *str, SD_RW_Data *p)
{
	sprintf(g_glcdBuf ,"SL %d", sizeof((*p).data)/sizeof(char) );
	putStringInGlcdAtPage(PAGE3, g_glcdBuf);
	if( ( (*p).MSB + strlen(str) ) <= ( (*p).data + sizeof((*p).data)/sizeof(char) ) )
	{
		strcpy((*p).MSB, str);
		(*p).MSB += strlen(str);
		return 0;
	}
	else
	{
		return -1;
	}
}

char inputSdCardBuffer(char *str, SD_RW_Data *p)
{
	sprintf(g_glcdBuf ,"SL %d", strlen(str));
	putStringInGlcdAtPage(PAGE2, g_glcdBuf);
	memset(p, 0x00, sizeof(SD_RW_Data));
	(*p).MSB = (*p).data;
	return addSdCardBuffer(str, p);
}
