#include <sdcard.h>

unsigned int crcResponse = 0;

unsigned char transSDcardCommand(unsigned char spiDeviceNumber, unsigned char cmd, unsigned long argument, unsigned char crc)
{
	unsigned char normalResponse=0;
	unsigned char retry=0;

	spiDeviceHold(spiDeviceNumber);

	spiTransCeive((0x40|cmd));

	spiTransCeive(argument>>24);
	spiTransCeive(argument>>16);
	spiTransCeive(argument>>8);
	spiTransCeive(argument);

	spiTransCeive(crc);
	//after, CRC is disabled

	retry=0;
	while((normalResponse = spiReceive()) == 0xFF)//waiting command response.
	{
		if(retry++>MAXIMUM_RETRY)
		{
			spiDeviceRelease();
			return normalResponse;
		}
	}

	switch(cmd)
	{
		//R1
		case GO_IDLE_STATE:				//0//R1	//CRC//0x95
		case SEND_OP_COND:				//1//R1
		case SWITCH_FUNC:				//6//R1
		case SEND_CSD:					//9//R1//DataRead
		case SEND_CID:					//10//R1//DataRead
		case SET_BLOCKLEN:				//16//R1
		case READ_SINGLE_BLOCK:			//17//R1//DataRead
		case READ_MULTIPLE_BLOCK:		//18//R1//DataRead
		case WRITE_BLOCK:				//24//R1
		case WRITE_MULTIPLE_BLOCK:		//25//R1
		case PROGRAM_CSD:				//27//R1
		case SEND_WRITE_PROT:			//30//R1
		case ERASE_WR_BLK_START_ADDR:	//32//R1
		case ERASE_WR_BLK_END_ADDR:		//33//R1
		case LOCK_UNLOCK:				//42//R1
		case APP_CMD:					//55//R1
		case GEN_CMD:					//56//R1
		case CRC_ON_OFF:				//59//R1

		case SD_SEND_OP_COND:			//ACMD41//R1

		//R1b
		case STOP_TRANSMISSION:			//12//R1b
		case SET_WRITE_PROT:			//28//R1b
		case CLR_WRITE_PROT:			//29//R1b
		case ERASE:						//38//R1b
			break;

		//R2
		case SEND_STATUS:				//13//R2
			while((normalResponse=spiReceive()) == 0xFF);
			break;

		//R3
		case READ_OCR:					//58//R3	//CRC:0x75

		//R7
		case SEND_IF_COND:				//8//R7	//CRC//0x87
			while((normalResponse=spiReceive()) == 0xFF);
			spiReceive();
			spiReceive();
			spiReceive();
			break;

		default:
			return -1;
	}

	switch(cmd)
	{
		case SEND_CSD:					//9//R1//DataRead
		case SEND_CID:					//10//R1//DataRead
		case READ_SINGLE_BLOCK:			//17//R1//DataRead
		case READ_MULTIPLE_BLOCK:		//18//R1//DataRead

		case WRITE_BLOCK:				//24//R1
		case WRITE_MULTIPLE_BLOCK:		//25//R1

			SPI_DEVICE_ENABLE;
			break;

		default:
			spiDeviceRelease();
	}
	return normalResponse;
}

unsigned char transSDcardCommand2(unsigned char spiDeviceNumber, unsigned char cmd, unsigned long argument, unsigned char crc)
{
	unsigned char normalResponse=0;
	unsigned int retry=0;


	spiDeviceHold(spiDeviceNumber);

	spiTransCeive((0x40|cmd));

	spiTransCeive(argument>>24);
	spiTransCeive(argument>>16);
	spiTransCeive(argument>>8);
	spiTransCeive(argument);

	spiTransCeive(crc);
	//after, CRC is disabled


	retry=0;
	while((normalResponse = spiReceive()) == 0xFF)//waiting command response.
	{
		if(retry++>MAXIMUM_RETRY*2)
		{
			spiDeviceRelease();

			return normalResponse;
		}
	}

	
	switch(cmd)
	{
		//R1, come out
		case SEND_CSD:					//9//R1//Data Receive
		case SEND_CID:					//10//R1//Data Receive
		case READ_SINGLE_BLOCK:			//17//R1//Data Receive
		case READ_MULTIPLE_BLOCK:		//18//R1//Data Receive

		case WRITE_BLOCK:				//24//R1//Data Send
		case WRITE_MULTIPLE_BLOCK:		//25//R1//Data Send
			break;
			

		//R1(8bits)
		case GO_IDLE_STATE:				//0//R1	//CRC//0x95
		case SEND_OP_COND:				//1//R1
		case SWITCH_FUNC:				//6//R1

		case SET_BLOCKLEN:				//16//R1


		case PROGRAM_CSD:				//27//R1
		case SEND_WRITE_PROT:			//30//R1
		case ERASE_WR_BLK_START_ADDR:	//32//R1
		case ERASE_WR_BLK_END_ADDR:		//33//R1
		case LOCK_UNLOCK:				//42//R1
		case APP_CMD:					//55//R1
		case GEN_CMD:					//56//R1
		case CRC_ON_OFF:				//59//R1

		case SD_SEND_OP_COND:			//ACMD41//R1
			
		
		//R1b(8bits)
		case STOP_TRANSMISSION:			//12//R1b
		case SET_WRITE_PROT:			//28//R1b
		case CLR_WRITE_PROT:			//29//R1b
		case ERASE:						//38//R1b
			break;

			
		//R2(16bits)
		case SEND_STATUS:				//13//R2
			while(spiReceive() == 0xFF);
			break;

		//R3(8bits)
		case READ_OCR:					//58//R3	//CRC:0x75

		//R7(32bits)
		case SEND_IF_COND:				//8//R7	//CRC//0x87
			while(spiReceive() == 0xFF);
			spiReceive();
			spiReceive();
			spiReceive();
			break;

		default:
			break;
	}

	spiDeviceRelease();
	return normalResponse;
}

/*Not suppored yet*/
unsigned int receiveSDcardDataMultiBlock(unsigned int getBlockNumber)
{
	return 0;
}

unsigned int receiveSDcardData(unsigned char cmd, SD_RW_Data *p, unsigned int readBytesLength, unsigned int long loadedBlockLength)//return value that is lastest crc.
{
//	unsigned char n=0;
	unsigned int crc=0;
	unsigned long getBlockLength = 0;
	SPI_DEVICE_ENABLE;//CD bit was "0"

	(*p).MSB = (*p).data;
	char *endOfMSB = (*p).data;


	while(getBlockLength<loadedBlockLength)
	{	

		while(spiReceive() != TOKEN_START_BLOCK);

		for(endOfMSB+=readBytesLength; (*p).MSB<(endOfMSB); (*p).MSB++)
		{
			*(*p).MSB = spiReceive();
		}
		getBlockLength++;

		crc = ( spiReceive()<<8 );
		crc |= spiReceive();
		switch(cmd)
		{
			case SEND_CSD://					9//R1//DataRead
			case SEND_CID://					10//R1//DataRead
			case READ_SINGLE_BLOCK:

				while( spiReceive() != 0xFF );
				spiDeviceRelease();
			default:
				return crc;

			case READ_MULTIPLE_BLOCK:
				break;
		}
	}
	while(spiTransCeive(STOP_TRANSMISSION) == 0xFD);
	while( spiReceive() != 0xFF );

	spiDeviceRelease();
	spiReceive();
	return crc;
}

unsigned char readSdcard(unsigned char cmd, SD_RW_Data *p, unsigned long blockAddr, unsigned int readBytesLength, unsigned int long loadedBlockLength)//return value that is lastest crc.
{
	unsigned char response;
	switch(cmd)
	{
		default:
			return -1;
		case SEND_CSD:
		case SEND_CID:
			readBytesLength = 16;
			loadedBlockLength = 1;
			break;
		case READ_SINGLE_BLOCK:
			readBytesLength = 512;
			loadedBlockLength = 1;
			break;
		case READ_MULTIPLE_BLOCK:// not supported, so same work READ_SINGLE_BLOCK
			readBytesLength = 512;
			loadedBlockLength = 1;			
	}
	response = transSDcardCommand(0, cmd, blockAddr, 0);//CSD, CID command ignore arguments
	receiveSDcardData(cmd ,p ,readBytesLength, loadedBlockLength);

	(*p).physicalSectorNumber = blockAddr;

	return response;
}

unsigned long find8bytes(char *findString, SD_RW_Data *p, unsigned long startBlockOffset)
{
	unsigned char response;
	char *charIndex = 0;
	unsigned char count=0;

	do
	{
		if( (response = readSdcard(READ_SINGLE_BLOCK, p, startBlockOffset++, 512, 1) ) == 0x00 )//return value that is lastest crc.
		{
			for(charIndex=(*p).data; charIndex<((*p).data+SD_DATA_BUFFER_SIZE); charIndex+=8)
			{
				count=0;
				while(*(charIndex+count) == *(findString+count))
				{
					if(!(++count<8)) break;
				}
				
				if(count==8) break;
			}

		}
		else
		{
			return -1;
		}
	}
	while((count != 8)&&(response == 0x00));

	return --startBlockOffset;
}

unsigned char transferSDcardData(unsigned char cmd, SD_RW_Data *p, unsigned int writeBytesLength, unsigned int long unLoadedBlockLength)//return value that is latest command response.
{
	unsigned long throwBlockLength = 0;
	unsigned char response = 0xFF;
	SPI_DEVICE_ENABLE;//CD bit was "0"


	char *sendOfMSB = (*p).data;
	char *endOfMSB = ((*p).data+writeBytesLength);


	while(spiReceive() != 0xFF);

	spiTransfer(TOKEN_START_BLOCK);

	while(throwBlockLength<unLoadedBlockLength)
	{	
		for(sendOfMSB = (*p).data; sendOfMSB<(*p).MSB; sendOfMSB++)
		{//sendOfMSB++
			spiTransfer(*sendOfMSB);
		}
		while(sendOfMSB < endOfMSB)
		{
			spiTransfer(0x00);
			sendOfMSB++;
		}

//	sendingCRC:
		spiTransfer(0xFF);//dummy MSB CRC transfer
		spiTransfer(0xFF);//dummy MSB LSB transfer

		response = spiReceive();//receive data response token

		switch(cmd)
		{
			case WRITE_BLOCK:
				switch( (response&TOKEN_DATA_BIT_WEIGHT) )//confirm data transfer is complete? with data transfer token.
				{
					default:
					case TOKEN_DATA_REJECT_CRC:
						break;
					case TOKEN_DATA_REJECT_WRITE_ERROR:
						break;

					case TOKEN_DATA_ACCEPT:
						break;
				}
				break;

			case READ_MULTIPLE_BLOCK:
				break;

			default:
				while(spiReceive() != 0xFF);
				return 0xFF;
		}


		throwBlockLength++;

	}

	while( (response = spiReceive()) == 0x00 );//wating busy.//sd card enter busy transfer mode, that is always send resonse busy response.

	while( spiReceive() != 0xFF );//spi commuication is ended, sd card response 0xFF.

	spiDeviceRelease();
	return response;
}

unsigned char writeSdcard(unsigned char cmd, SD_RW_Data *p, unsigned long blockAddr, unsigned int writeBytesLength, unsigned int long unLoadedBlockLength)//return value that is lastest crc.
{
	unsigned char response;
	switch(cmd)
	{
		default:
			return -1;
			break;
		case WRITE_BLOCK:
			writeBytesLength = 512;
			unLoadedBlockLength = 1;
			break;
		case WRITE_MULTIPLE_BLOCK:// not supported, so same work READ_SINGLE_BLOCK
			writeBytesLength = 512;
			unLoadedBlockLength = 1;			
	}

	if( (response = transSDcardCommand(0, cmd, blockAddr, 0)) == 0x00)//CSD, CID command ignore arguments
	{
		response = transferSDcardData(cmd ,p , writeBytesLength, unLoadedBlockLength);
	}
	else
	{
		spiDeviceRelease();
		response = spiReceive();
	}

	return response;
}
