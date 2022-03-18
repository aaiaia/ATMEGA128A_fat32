#ifndef _UART_H_
#define _UART_H_

//Using Interrupt start
/*
Baud Rate List	UBRR(UBRRnH,UBRRnL,U2X=0)
2400UL
4800UL
9600UL
14400UL
19200UL
28800UL
38400UL
57600UL
76800UL
115200UL
230400UL
250000UL
500000UL
1000000UL

asynchronization
UBRR Formula(U2X=0): UBRR = (f_osc / 16*BAUR_RATE)-1
UBRR Formula(U2X=1): UBRR = (f_osc / 8*BAUR_RATE)-1
*/
#if(F_CPU == 16000000UL)
//	#define BAUD_RATE	9600UL//57600UL

	#define BAUD_RATE	57600UL
//	#define BAUD_RATE	115200UL
#elif(F_CPU == 8000000UL)
	#define BAUD_RATE	28800UL//57600UL
#else
	#define BAUD_RATE	28800UL
#endif
/*UCSR0A*/
//ºñÆ®ÀÇ À§Ä¡ ¾Æ·¡´Â UCSRnA¸¦ ±¸¼ºÇÏ´Â ºñÆ®ÀÇ À§Ä¡
//RXCn   TXCn   UDREn  FEn   DORn  UPEn   U2Xn   MPCMn
//UPEn : Perity bit
//FEn : Frame error µ¥ÀÌÅÍ ¼ö½Å ÀÌÈÄ Á¤Áö ºñÆ®¿¡¼­´Â 1À» ¼ö½ÅÇØ¾ßÇÏ´Âµ¥, ÀÌ°¡ Á¤»óÀûÀ¸·Î ¼öÇàµÇÁö ¾ÊÀ¸¸é
//			FE°¡ 1·Î setÀÌ µÈ´Ù
//DOR(OVR) : ¼ö½ÅµÈ ¹®ÀÚ´Â  UDR ·¹Áö½ºÅÍ¿¡ ÀúÀå, ÀÌ´Â 2°³°¡ Á¸Àç, ¸¸¾à 2°³ÀÇ UDR ·¹Áö½ºÅÍ°¡ Â÷ ÀÖ´Â »óÅÂ¿¡¼­ ¼ö½ÅµÉ °æ¿ì
//			DORÀº 1·Î set µÊ
/*UCSR0A*/
#define	RXC		7
#define TXC		6
#define	UDRE	5
#define FE		4
#define DOR 	3
#define UPE		2
#define U2X		1

/*UCSR0B*/
//RXCIEn : UDR¿¡ ¼ö½ÅµÈ ¹®ÀÚ°¡ ÀÖÀ¸¸é USCRnAÀÇ RXC°¡ 1, ±×¸®°í ³­ ÈÄ ¼ö½Å ÀÎÅÍ·´Æ® ¹ß»ý
//TXCIEn : ¼Û½ÅÀÌ ¿Ï·áµÉ ¶§ USCRnA ·¹Áö½ºÅÍ¿¡ TXC ÇÃ·¡±× 1, ±×¸®°í ³­ ÈÄ ¼Û½Å ÀÎÅÍ·´µå ¹ß»ý
//UDRIEn : ¼Û½Å UDR ·¹Áö½ºÅÍ°¡ ºñ°ÔµÉ ¶§ USCRnA ·¹Áö½ºÅÍÀÇ URDREn ÇÃ·¡±×ÀÌ µÊ
//RXENn : USARTn ¼ö½Å ±â´É È°¼ºÈ­
//TXENn : USAERn ¼Û½Å±â´É È°¼ºÈ­
/*UCSR0B*/
#define RXCIE	7
#define TXCIE	6
#define UDRIE	5
#define RXEN	4
#define TXEN	3
#define UCSZ2	2//data bit size
#define RXB8	1
#define TXB8	0

/*UCSR0C*/
//UMSELn : µ¿±â ºñµ¿±â
//UPMn1 : ÆÐ¸®Æ¼ ºñÆ® ¼³Á¤
//UPMn0 : ÆÐ¸®Æ¼ ºñÆ® ¼³Á¤
//USBSn : Á¤Áö ºñÆ®ÀÇ °¹¼ö
//UCZSn1 : µ¥ÀÌÅÍ ºñÆ® °¹¼ö
//UCSZn0 : µ¥ÀÌÅÍ ºñÆ® °¹¼ö
//UCPOLn : Å¬·Ï »ùÇÃ¸µ À§Ä¡
/*UCSR0C*/
#define UMSEL	6
#define UPM1	5
#define UPM0	4
#define USBS	3
#define UCSZ1	2
#define UCSZ0	1
#define UCPOL	0


/*ERROR DETECTION*/
#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

#define GLOBAL_ERROR_CHECK (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)
		/*not enough .data size. not used start*/

		// USART0 Receiver buffer
		/*
		#define TRX_BUFFER_SIZE 255
		char rxBuffer0[TRX_BUFFER_SIZE] = {0};
		char txBuffer0[TRX_BUFFER_SIZE] = {0};
		*/
		/*not enough .data size. not used end*/
#define	RX_END_WORD	'\0'
#define	TX_END_WORD	'\0'
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
