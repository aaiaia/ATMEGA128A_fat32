#include <sdcard_test.h>
	/*Invalid CMD response test code start*/
	/*
		sprintf(g_glcdBuf ,"invalied CMD response");
		putStringInGlcdAtPage(PAGE1, g_glcdBuf);
		sprintf(g_glcdBuf ,"0x%02x%02x%02x%02x %02x%02x%02x%02x", spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive());
		putStringInGlcdAtPage(PAGE2, g_glcdBuf);
		sprintf(g_glcdBuf ,"0x%02x%02x%02x%02x %02x%02x%02x%02x", spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive());
		putStringInGlcdAtPage(PAGE3, g_glcdBuf);
		sprintf(g_glcdBuf ,"0x%02x%02x%02x%02x %02x%02x%02x%02x", spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive());
		putStringInGlcdAtPage(PAGE4, g_glcdBuf);
		sprintf(g_glcdBuf ,"0x%02x%02x%02x%02x %02x%02x%02x%02x", spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive());
		putStringInGlcdAtPage(PAGE5, g_glcdBuf);
		sprintf(g_glcdBuf ,"0x%02x%02x%02x%02x %02x%02x%02x%02x", spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive());
		putStringInGlcdAtPage(PAGE6, g_glcdBuf);
		sprintf(g_glcdBuf ,"0x%02x%02x%02x%02x %02x%02x%02x%02x", spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive());
		putStringInGlcdAtPage(PAGE7, g_glcdBuf);
		sprintf(g_glcdBuf ,"0x%02x%02x%02x%02x %02x%02x%02x%02x", spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive(), spiReceive());
		putStringInGlcdAtPage(PAGE8, g_glcdBuf);
																										nextSequence();
	*/
	/*Invalid CMD response test code end*/

void testGlcdHexPrint(SD_RW_Data *p, unsigned int bytes, unsigned char mode)
{
	bytes = bytes/8;

	char *i = (*p).data;
	unsigned char page = 4;
	unsigned char line = 0;


	sprintf(g_glcdBuf, "P.S.N %000000008lx", (*p).physicalSectorNumber);
	putStringInGlcdAtPage(PAGE(page++), g_glcdBuf);

	for(line=0; ((line<bytes)&&(line<sizeof((*p).data))); line++)
	{
		if(page > 8)
		{
											if(mode == TOGGLED)
											{
																					if(nextSequence() == '2')
																					{
																						return;
																					}
											}
			//page = 4;
			page = 5;
		}
			sprintf(g_glcdBuf, "%02x", *i);
			i++;
			sprintf(g_glcdBuf ,"%s%02x", g_glcdBuf, *i);
			i++;
			sprintf(g_glcdBuf ,"%s%02x", g_glcdBuf, *i);
			i++;
			sprintf(g_glcdBuf ,"%s%02x", g_glcdBuf, *i);
			i++;

			sprintf(g_glcdBuf ,"%s %02x", g_glcdBuf, *i);
			i++;
			sprintf(g_glcdBuf ,"%s%02x", g_glcdBuf, *i);
			i++;
			sprintf(g_glcdBuf ,"%s%02x", g_glcdBuf, *i);
			i++;
			sprintf(g_glcdBuf ,"%s%02x %03d", g_glcdBuf, *i, line);
			i++;

//			sprintf(g_glcdBuf, "%02x%02x %02x%02x%02x%02x", *i++, *i++, *i++, *i++, *i++, *i++, *i++, *i++);//8bit
			putStringInGlcdAtPage(PAGE(page++), g_glcdBuf);
	}
	while(page < 9)
	{
		putStringInGlcdAtPage(PAGE(page++), "                                 ");
	}

											if(mode == TOGGLED)
											{
																					if(nextSequence() == '2') return;
											}
}

void testSdCardTracer(char *title, unsigned char times)
{
	unsigned char i,j;
	sprintf(g_glcdBuf, "%s", title);
	putStringInGlcdAtPage(PAGE1, g_glcdBuf);

	for(i=0; i<times; i++)
	{
		for(j=0; j<7; j++)
		{
		sprintf(g_glcdBuf, "%02x", spiReceive());
		sprintf(g_glcdBuf, "%s%02x",g_glcdBuf, spiReceive());
		sprintf(g_glcdBuf, "%s%02x",g_glcdBuf, spiReceive());
		sprintf(g_glcdBuf, "%s%02x",g_glcdBuf, spiReceive());
		sprintf(g_glcdBuf, "%s%02x",g_glcdBuf, spiReceive());
		sprintf(g_glcdBuf, "%s%02x",g_glcdBuf, spiReceive());
		sprintf(g_glcdBuf, "%s%02x",g_glcdBuf, spiReceive());
		sprintf(g_glcdBuf, "%s%02x",g_glcdBuf, spiReceive());
		sprintf(g_glcdBuf, "%s%02x",g_glcdBuf, spiReceive());
		sprintf(g_glcdBuf, "%s%02x",g_glcdBuf, spiReceive());
		putStringInGlcdAtPage(PAGE0+1+j, g_glcdBuf);
		}
		nextSequence();
	}
}
