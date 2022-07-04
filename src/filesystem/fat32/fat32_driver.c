char SPI_SD_CARD_Init(unsigned char spiDeviceNumber)
{
	unsigned char retry=0;
	unsigned char responseSet;


	SPI_SD_CARD_SPCR_SET;


	if(flag_sd_card_status & SD_CARD_INITIALIZATION)
	{
		retry = 0;
		
		spiDeviceHold(spiDeviceNumber);
		
		do
		{
			responseSet = transSDcardCommand(0, APP_CMD, 0, 0);//APP_CMD//CMD55
			responseSet = transSDcardCommand(0, SD_SEND_OP_COND, 0x40000000, 0);//ACMD41		0x3A//dec ACMD41d

			if(retry++==0xFF)
			{
				flag_sd_card_status &= ~SD_CARD_INITIALIZATION;
				spiDeviceRelease();
				return -1;
			}
		}
		while( (responseSet == 0x01)&&(responseSet != 0x00) );


		SPI_SD_CARD_SPCR_SET;
		SPI_SD_CARD_SPSR_SET_HIGH_SPEED;

		spiDeviceRelease();
		return 0;
	}

	#if (F_CPU == 16000000UL)
		SPI_SD_CARD_SPCR_SET;
		SPCR |= ((1<<SPR1)|(1<<SPR0));
		SPSR &= (~(1<<SPI2X));
//			flag_sd_card_status |= SD_CARD_INITIALIZATION;
	#elif (F_CPU == 8000000UL)
		SPI_SD_CARD_SPCR_SET;
		SPCR |= (1<<SPR1);
		SPSR &= (~(1<<SPI2X));
//			flag_sd_card_status |= SD_CARD_INITIALIZATION;
	#else
		spiDeviceRelease();
		return -1;
	#endif

	spiDeviceHold(spiDeviceNumber);

	retry = 0;
	while( (responseSet = transSDcardCommand(0, GO_IDLE_STATE, 0, 0x95)) != 0x01 )
	{
		if(retry++ > 254)
		{
			flag_sd_card_status &= ~SD_CARD_INITIALIZATION;
			spiDeviceRelease();
			return -1;
		}
	
		unsigned char count;
		for(count=0; count<10; count++)
		{
			spiTransfer(0xFF);//clk generation, 80clk
		}
	}
	//SDHC SANDISK 4GB response(response Type is R1, response length 8bits(1byte, hex code weight 2) ): 0x 01

	responseSet = transSDcardCommand(0, SEND_IF_COND, 0x1AA, 0x87);//command response is 0x01. //SEND_IF_COND//CMD8
	//SDHC SANDISK 4GB response(response Type is R7, response length 48bits(6byte, hex code weight 12) ): 0x 01 00 00 01 aa ff
	retry=0;
	while(spiReceive() != 0xFF)
	{
		if(retry++ == 0xFF)
		{
			flag_sd_card_status &= ~SD_CARD_INITIALIZATION;
			spiDeviceRelease();
			return -1;
		}
	}

	responseSet = transSDcardCommand(0, READ_OCR, 0, 0x75);//READ_OCR		0x3A//dec CMD58//0x75
	//SDHC SANDISK 4GB response(response Type is R3, response length 40bits(5byte, hex code weight 10) ): 0x 01 00 ff 80 00

	retry = 0;
	do
	{
		responseSet = transSDcardCommand(0, APP_CMD, 0, 0);//APP_CMD//CMD55

		responseSet = transSDcardCommand(0, SD_SEND_OP_COND, 0x40000000, 0);//ACMD41		0x3A//dec ACMD41d

		if(retry++==0xFF)
		{
			flag_sd_card_status &= ~SD_CARD_INITIALIZATION;
			spiDeviceRelease();
			return -1;
		}
	}
	while( responseSet == 0x01 );
	//end initialization & indentification

	sdCardTestCheck= 5;//to test.
	responseSet = transSDcardCommand(0, READ_OCR, 0, 0x75);//READ_OCR		0x3A//dec CMD58//0x75
	//SDHC SANDISK 4GB response(response Type is R3, response length 40bits(5byte, hex code weight 10) ): 0x 00 c0 ff 80 00

	SPI_SD_CARD_SPCR_SET;
	SPI_SD_CARD_SPSR_SET_HIGH_SPEED;
	flag_sd_card_status |= SD_CARD_INITIALIZATION;

	spiDeviceRelease();
	return 0;
}
