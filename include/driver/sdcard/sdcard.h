#ifndef _SDCARD_H_
#define _SDCARD_H_
#define SD_CARD_INITIALIZATION	(1<<0)

unsigned char flag_sd_card_status = 0x00;
unsigned long int sdCardTestCheck = 0;
/*
	Description of flag of sd card
	MSB
	7	SDHC
	6	reserved
	5	reserved
	4	reserved
	3	reserved
	2	reserved
	1	reserved
	0	SD card was initialized//high OK, low NOT
	LSB
*/

//CMD
#define GO_IDLE_STATE				0//R1	//CRC//0x95
#define SEND_OP_COND				1//R1
#define SWITCH_FUNC					6//R1
#define SEND_IF_COND				8//R7	//CRC//0x87
#define	SEND_CSD					9//R1//DataRead
#define	SEND_CID					10//R1//DataRead
#define STOP_TRANSMISSION			12//R1b
#define SEND_STATUS					13//R2
#define SET_BLOCKLEN				16//R1
#define READ_SINGLE_BLOCK			17//R1//DataRead
#define READ_MULTIPLE_BLOCK			18//R1
#define WRITE_BLOCK					24//R1
#define	WRITE_MULTIPLE_BLOCK		25//R1
#define	PROGRAM_CSD					27//R1
#define SET_WRITE_PROT				28//R1b
#define CLR_WRITE_PROT				29//R1b
#define SEND_WRITE_PROT				30//R1
#define ERASE_WR_BLK_START_ADDR		32//R1
#define ERASE_WR_BLK_END_ADDR		33//R1
#define ERASE						38//R1b
#define	LOCK_UNLOCK					42//R1
#define APP_CMD						55//R1
#define GEN_CMD						56//R1
#define READ_OCR					58//R3	//CRC:0x75
#define CRC_ON_OFF					59//R1
//ACMD
#define SD_SEND_OP_COND				(41|0x40)//dec ACMD41

//Tokens
//Data Response Token
/*Every data block writtento the card will be acknowledged by a data response token. it is one byte long and has the following format.*/
#define TOKEN_DATA_BIT_WEIGHT				0x1F
#define TOKEN_DATA_ACCEPT					0x05
#define TOKEN_DATA_REJECT_CRC				0x0B
#define TOKEN_DATA_REJECT_WRITE_ERROR		0x0D
/*In case of any error(CRC or Write Error) during Write Multipe Block operation, the host shall stop the data transmission using CMD12.
In case of a Write Error(response '110'), the host may send CMD13(SNED_STATUS) in order to get the casue of the write problem.
ACMD22 can be used to find the number of well written write blocks.*/

//Start and stop Tran token
/*Read and write commands have dat transfer associated with them. Data is being trasnmitted or received via data tokens.
All data bytes are transmitted MSB first. Data token are 4 to 515 bytes long and have the following format.*/
#define TOKEN_START_BLOCK					0xFE
#define TOKEN_MULTIPLE_WRITE_BLOCK			0xFC
#define TOKEN_STOP_TRANSMISSION				0xFD
/*Note that this format is used only for Multiple Block Write. In case of a Multiple Block Read the stop transmissin is performedusing STOP_TRAN Command(CMD12).*/

//Data Error Token
/*If a read operation fails and the card canot provide the required data, it will send a data error token instead. This token isone byte long and gas the following format.*/
#define TOKEN_ERROR							0x01
#define TOKEN_ERROR_CC						0x02
#define TOKEN_ERROR_ECC_FAIL				0x04
#define TOKEN_ERROR_OUT_RANGE				0x08

#define MAXIMUM_RETRY	0xFE

#define SD_DATA_BUFFER_SIZE		0x200

#define PHYSICAL_SECTER_NUMBER	(unsigned long)//

struct SD_RW_Data
{
	unsigned long physicalSectorNumber;
	unsigned char responseSet;
	char data[SD_DATA_BUFFER_SIZE];
	char *MSB;
}typedef SD_RW_Data;
#endif
