#include "keypad.h"

void reInitKeyPad()
{
	DDRA |= 1<<KEY_OUT_LEFT;
	DDRD |= 1<<KEY_OUT_CENTER;
	DDRD |= 1<<KEY_OUT_RIGHT;

	DDRD &= ~(1<<KEY_IN_TOP);
	DDRD &= ~(1<<KEY_IN_BOTTOM);

	PORTD |= (1<<KEY_IN_TOP);
	PORTD |= (1<<KEY_IN_BOTTOM);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char nextSequence()
{
	// unsigned char i=0;

	char KeyBuf=0xFF;  // Å° °ªÀÌ µé¾î°¥ ¹öÆÛ, ÃÊ±â°ª 0xFF

	while(KeyBuf == 0xFF)
	{

		reInitKeyPad();//20140822 ÅÂ¼º

		PORTA&=~0b00000010; _delay_us(5); // 1¹øÂ° ÁÙ ¼±ÅÃ Ãâ·Â ºÎºÐ ÀÔ·Â.

		
		
		if ((PIND&0x10)==0)KeyBuf='0'; // ÀÔ·Â ºÎºÐ PD4
		if ((PIND&0x20)==0)KeyBuf='4';
		//if ((PIND&0x01)==0)KeyBuf='8'; // PD0
		PORTA|=0b00000010; // 1¹øÂ° ÁÙ ÇØÁ¦
		//PD6
		PORTD&=~0b01000000; _delay_us(5); // 1¹øÂ° ÁÙ ¼±ÅÃ Ãâ·Â ºÎºÐ ÀÔ·Â.



		if ((PIND&0x10)==0)KeyBuf='2'; // ÀÔ·Â ºÎºÐ
		if ((PIND&0x20)==0)
		{
			KeyBuf='6';
			displayDs1302ReadTime();
			KeyBuf=0xFF; // PD5
		}
		//if ((PIND&0x01)==0)KeyBuf='3';
		PORTD|=0b01000000; // 1¹øÂ° ÁÙ ÇØÁ¦
		//PD7
		PORTD&=~0b10000000; _delay_us(5); // 1¹øÂ° ÁÙ ¼±ÅÃ Ãâ·Â ºÎºÐ ÀÔ·Â.

		
		
		if ((PIND&0x10)==0)KeyBuf='1'; // ÀÔ·Â ºÎºÐ
		if ((PIND&0x20)==0)KeyBuf='5'; // PD5


		//if ((PIND&0x01)==0)KeyBuf='7';
		PORTD|=0b10000000; // 1¹øÂ° ÁÙ ÇØÁ¦

		//PORTD = 0b00110001; DDRA = 0b11111111; DDRD = 0b11001110;  //20140822 ¼öÁ¤
		PORTD |= 0b00110001; DDRA |= 0b00000001; DDRD = 0b11001110; // 20140822 taesung
		_delay_ms(100);



//		if(KeyBuf == 0xff) 	putStringInGlcdAtPage(PAGE8, "");
	}
	return KeyBuf; // Key ¾øÀ¸¸é 0xFF ¸®ÅÏ
}
