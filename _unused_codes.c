#define PREV_PAGE	0xF0
#define NEXT_PAGE	0x0F
#define ENTER_PAGE	0x3C
#define END_DISPLAY	0xFF
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char directionOfPage()
{
		//overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE1, 115, "");
		//overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE8, 115, "");
		putByteDataGlcd(0, 122, 0x00);
		putByteDataGlcd(0, 123, 0x08);
		putByteDataGlcd(0, 124, 0x0c);
		putByteDataGlcd(0, 125, 0x7e);
		putByteDataGlcd(0, 126, 0x0c);
		putByteDataGlcd(0, 127, 0x08);

		putByteDataGlcd(1, 122, 0x00);
		putByteDataGlcd(1, 123, 0x1e);
		putByteDataGlcd(1, 124, 0x10);
		putByteDataGlcd(1, 125, 0x7c);
		putByteDataGlcd(1, 126, 0x38);
		putByteDataGlcd(1, 127, 0x10);

		putByteDataGlcd(2, 122, 0x00);
		putByteDataGlcd(2, 123, 0x10);
		putByteDataGlcd(2, 124, 0x30);
		putByteDataGlcd(2, 125, 0x7e);
		putByteDataGlcd(2, 126, 0x30);
		putByteDataGlcd(2, 127, 0x10);

		putByteDataGlcd(3, 122, 0x00);
		putByteDataGlcd(3, 123, 0x42);
		putByteDataGlcd(3, 124, 0x24);
		putByteDataGlcd(3, 125, 0x18);
		putByteDataGlcd(3, 126, 0x24);
		putByteDataGlcd(3, 127, 0x42);

		putByteDataGlcd(4, 122, 0x00);
		putByteDataGlcd(4, 123, 0x7c);
		putByteDataGlcd(4, 124, 0x42);
		putByteDataGlcd(4, 125, 0x7e);
		putByteDataGlcd(4, 126, 0x44);
		putByteDataGlcd(4, 127, 0x78);

		putByteDataGlcd(5, 122, 0x00);
		putByteDataGlcd(5, 123, 0x7e);
		putByteDataGlcd(5, 124, 0x42);
		putByteDataGlcd(5, 125, 0x4e);
		putByteDataGlcd(5, 126, 0x4c);
		putByteDataGlcd(5, 127, 0x78);

		while(1)
		{
			SWITCH = nextSequence();

			_delay_ms(200);
			switch(SWITCH)
			{
				case '5':
					return PREV_PAGE;
				case '6':
					return ENTER_PAGE;
				case '1':
					return NEXT_PAGE;
				case '4':
					return END_DISPLAY;
				default:
					continue;
			}
		}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char selectPage()
{
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE1, 120, "1");
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE2, 120, "2");
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE3, 120, "3");
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE4, 120, "4");
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE5, 120, "5");
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE6, 120, "6");
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE7, 120, "7");
		overwriteStringInGlcdAtPageAddressSetStartColumnOffsetToPage(PAGE8, 120, "8");

		while(1)
		{
			SWITCH = PINA;
			_delay_ms(200);
			switch(SWITCH)
			{
				case 0x01:
					return 0;
				case 0x02:
					return 1;
				case 0x04:
					return 2;
				case 0x08:
					return 3;
				case 0x10:
					return 4;
				case 0x20:
					return 5;
				case 0x40:
					return 6;
				case 0x80:
					return 7;

				default:
					continue;
			}
		}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
