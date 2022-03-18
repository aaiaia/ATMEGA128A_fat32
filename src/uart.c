#include "uart.c"

void initUsart0()
{
	/* for USART */
//0000 0000			RXCn   TXCn   UDREn  FEn   DORn  UPEn   U2Xn   MPCMn
//	UCSR0A = 0x02; // U2X = 1; // 0000 0010
	UCSR0A &= ~(1<<U2X);//1<<U2X; // U2X = 1; // 0000 0010

//U2Xn : ºñµ¿±â Åë½Å¿¡¼­ 2¹è¼Ó Å¬·Ï

//RXCIEn TXCIEn UDRIEn RXENn TEXNn UCSZn2 RXB8n  TXB8n
//	UCSR0B = 0xF8; //1111 1000 // tranfer and receive enable

//*RXCIEn TXCIEn UDRIEn *RXENn *TXENn UCSZn2 RXB8n  TXB8n
//	UCSR0B = (1<<RXCIE | 1<<RXEN | 1<<TXEN); //1001 1000 // tranfer and receive enable

//*RXCIEn TXCIEn UDRIEn *RXENn *TXENn UCSZn2 RXB8n  TXB8n
	UCSR0B = (1<<RXCIE | 1<<TXCIE | 1<<RXEN | 1<<TXEN); //1101 1000 // tranfer and receive enable

//RXCIEn : UDR¿¡ ¼ö½ÅµÈ ¹®ÀÚ°¡ ÀÖÀ¸¸é USCRnAÀÇ RXC°¡ 1
//TXCIEn : ¼Û½ÅÀÌ ¿Ï·áµÉ ¶§ USCRnA ·¹Áö½ºÅÍ¿¡ TXC ÇÃ·¡±× 1
//UDRIEn : ¼Û½Å UDR ·¹Áö½ºÅÍ°¡ ºñ°ÔµÉ ¶§ USCRnA ·¹Áö½ºÅÍÀÇ URDREn ÇÃ·¡±×ÀÌ µÊ
//RXENn : USARTn ¼ö½Å ±â´É È°¼ºÈ­
//TXENn : USAERn ¼Û½Å±â´É È°¼ºÈ­

//0000 0110 NULL   UMSELn UPMn1  UPMn0 USBSn UCSZn1  UCSZn0 UCPOLn
//	UCSR0C = 0x06; //Asyncronous - no parity - 1bits(stop) - 8bits(data) // 0(no mean)0(async, sync)00(parity mode) 0(stop bit)11(data bit)0(positive, negative edge)
	UCSR0C = (1<<UCSZ1 | 1<<UCSZ0); //Asyncronous - no parity - 1bits(stop) - 8bits(data) // 0(no mean)0(async, sync)00(parity mode) 0(stop bit)11(data bit)0(positive, negative edge)
//UMSELn : µ¿±â ºñµ¿±â
//UPMn1 : ÆÐ¸®Æ¼ ºñÆ® ¼³Á¤
//UPMn0 : ÆÐ¸®Æ¼ ºñÆ® ¼³Á¤
//USBSn : Á¤Áö ºñÆ®ÀÇ °¹¼ö
//UCZSn1 : µ¥ÀÌÅÍ ºñÆ® °¹¼ö
//UCSZn0 : µ¥ÀÌÅÍ ºñÆ® °¹¼ö
//UCPOLn : Å¬·Ï »ùÇÃ¸µ À§Ä¡

/*

//0000 0000			RXCn   TXCn   UDREn  FEn   DORn  UPEn   U2Xn   MPCMn
//	UCSR0A = 0x02; // U2X = 1; // 0000 0010
	UCSR0A = (0<<U2X);//1<<U2X; // U2X = 1; // 0000 0010

ex)	#define F_CPU 16000000UL//CPU clock definition
ex)	#define BAUD_RATE	115200UL

	asynchronization
	UBRR Formula(U2X=0): UBRR = (f_osc / 16*BAUR_RATE)-1
	UBRR Formula(U2X=1): UBRR = (f_osc / 8*BAUR_RATE)-1

*/
	unsigned int ubrr = 0;
	if((UCSR0A&(1<<U2X)) == 0x00)//is mean that not using double rate
	{
		ubrr = ((F_CPU/(16*BAUD_RATE))-1);
	}
	else
	{
		ubrr = ((F_CPU/(8*BAUD_RATE))-1);
	}
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)(ubrr);

/*
	UBRR0H = 0;
	UBRR0L = 8; // 115k with U2X = 0
	UCSR0A = 0x00; // U2X = 0;
	UCSR0B = 0xF8;
	UCSR0C = 0x06; //Asyncronous - no parity - 1bits(stop) - 8bits(data)
*/
// Global enable interrupts
//#asm("sei")//<<CodeVision
}

void sendCommon()
{
	UDR0 = '\r';
	while (!(UCSR0A & SEND_IS_DONE));
	UDR0 = '\n';
	while (!(UCSR0A & SEND_IS_DONE));
	UDR0 = '\0';
	while (!(UCSR0A & SEND_IS_DONE));
}

void sendStringOnly(char *p)
{
	while(*p != '\0')
	{
		UDR0 = *p;
		while (!(UCSR0A & SEND_IS_DONE));
		p++;
	}
}
// char* receiveStringOnly(char *p)
// {
	// do
	// {
		// while( !(UCSR0A&(1<<RXC)) );
		// *p=UDR0;
	// }
	// while(*p++!='\n');

	// return p;
// }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendString(char *p)
{
	while(*p != '\0')
	{
		UDR0 = *p;
		while (!(UCSR0A & SEND_IS_DONE));
		p++;
	}
	sendCommon();
}
char* receiveString(char *p)
{
	do
	{
		while( !(UCSR0A&(1<<RXC)) );
		*p=UDR0;
	}
	while(*p++!='\n');

	(*p)=0;

	return p;
}
void sendChar(char p)
{
	UDR0 = p;
	while (!(UCSR0A & SEND_IS_DONE));
	sendCommon();
}
void sendCharOnly(char p)
{
	UDR0 = p;
	while (!(UCSR0A & SEND_IS_DONE));
}
