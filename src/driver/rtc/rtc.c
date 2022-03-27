#include "rtc.h"

void bsp_ds1302_gpio_init(void)
{
	//  the clock terminal (RTC_CLK) data terminal (RTC_DATA) Chip select (RTC_CS) is set to output
	DS1302_DDR |=_BV(DS1302_SCLK_PIN_NO)|_BV(DS1302_IO_PIN_NO)|_BV(DS1302_RST_PIN_NO);
}

/*******************************************
Function name: DS1302_write
Features: an address to write to the DS1302 a byte of data
Parameters: REG - address value (register or RAM)
data - to be written
Return Value: None
*******************************************/
void ds1302_write(unsigned char add , unsigned char data)
{
	unsigned char i=0;

	set_ds1302_io_ddr();     // configure the IO is the output
	delay_us(2);
	clr_ds1302_rst();        // clear reset, stop all operations
	delay_us(2);
	clr_ds1302_sclk();       // clear the clock, ready to operate
	delay_us(2);
	set_ds1302_rst();        // set reset to start operation
	delay_us(2);

	for(i=8;i>0;i--)         // This loop writes the control code
	{
		if(add&0x01)
			set_ds1302_io();     // this bit is 1, set the data bits
		else
			clr_ds1302_io();     // this bit is 0, clear data bits
			
		delay_us(2);
		set_ds1302_sclk();     // generates a clock pulse, the write data
		delay_us(2);
		clr_ds1302_sclk();
		delay_us(2);
		add>>=1;         // shift, ready to write the next one
	}
	for(i=8;i>0;i--)      // write code for this recycling
	{
		if(data&0x01)
			set_ds1302_io();
		else
			clr_ds1302_io();

		delay_us(2);
		set_ds1302_sclk();
		delay_us(2);
		clr_ds1302_sclk();
		delay_us(2);
		data>>=1;
	}

	clr_ds1302_rst(); 
	delay_us(2);
	clr_ds1302_io_ddr();      // clear output state
	delay_us(2);
}

/*******************************************
Function name: DS1302_read
Features: an address from the DS1302 to read one byte of data
Parameters: add - address value (register or RAM)
Return Value: data - the data read out
********************************************/
unsigned char ds1302_read(unsigned char add)
{
	unsigned char i=0,data=0;

	add+=1;                  // read flag
	set_ds1302_io_ddr();     // output ports
	delay_us(2);
	clr_ds1302_rst();        // clear reset
	delay_us(2);
	clr_ds1302_sclk();       // clear clock
	delay_us(2);
	set_ds1302_rst();        // Set Reset
	delay_us(2);

	for(i=8;i>0;i--)         // This loop writes the address code
	{
		if(add&0x01)
		{
			set_ds1302_io();
		}
		else
		{
			clr_ds1302_io();
		}

		delay_us(2);
		set_ds1302_sclk();
		delay_us(2);
		clr_ds1302_sclk();
		delay_us(2);
		add>>=1;
	}
	clr_ds1302_io_ddr();      // input port
	delay_us(2); 
	for(i=8;i>0;i--)         // This loop reads data from 1302
	{
		data>>=1; 
		if(in_ds1302_io())
		{
			data|=0x80;
		}
		delay_us(2);
		set_ds1302_sclk();
		delay_us(2);
		clr_ds1302_sclk();
		delay_us(2);
	}

	clr_ds1302_rst();
	delay_us(2);

	return(data);
}

/*******************************************
Function name: check_DS1302
Function: detect DS1302 is working
Parameters: None3

Return Value: exist - to TRUE for the detection of the DS1302, as not detected to FALSE
********************************************/
unsigned char check_ds1302(void)
{
	ds1302_write(ds1302_control_add,0x80);

	if(ds1302_read(ds1302_control_add)==0x80)
		return 1;
	
	return 0;
}

/*******************************************
Function name: DS1302_setT
Function: Set the DS1302 time
Parameters: ptTimeD - an array of pointers to set the time
Return Value: None
********************************************/
void ds1302_set_time(unsigned char set_time[])
{
	unsigned char i;
	unsigned char addr = 0x80;		//  seconds register write address from the beginning
	
	ds1302_write(ds1302_control_add|0x00,0x00);	// control commands, allowing a write operation
	delay_us(5000);
	
	for(i=0;i<7;i++)
	{
		ds1302_write(addr|0x00,set_time[i]);	// second week of sun and the moon in time-sharing
		addr+=2;
		delay_us(1000);
	}
	ds1302_write(ds1302_control_add|0x00,0x80);		// control commands, WP bit is 1, does not allow write operation
}

#if 0
/*******************************************
Function name: ds1302_write_time
Function: Clock data write to 1302
Parameters: None
Return Value: None
********************************************/
void ds1302_write_time(void)
{
	ds1302_write(ds1302_control_add, 0x00); // close the write protection
	
	ds1302_write(ds1302_sec_add, 0x80); // pause
	
	ds1302_write(ds1302_charger_add, 0x90); // trickle charge
	
	ds1302_write(ds1302_year_add, ds1302_time[0]); // years
		eeprom_write(0,buffer[0]);
	ds1302_write(ds1302_month_add, ds1302_time[1]); // month
		eeprom_write(1,buffer[1]);
	ds1302_write(ds1302_date_add, ds1302_time[2]); // day
		eeprom_write(2,buffer[2]);
	//ds1302_write(ds1302_day_add, ds1302_time[6]); // weeks
		eeprom_write(6,buffer[6]);
	ds1302_write(ds1302_hr_add, ds1302_time[3]); // time
		eeprom_write(3,buffer[3]);
	ds1302_write(ds1302_min_add, ds1302_time[4]); // min
		eeprom_write(4,buffer[4]);
	ds1302_write(ds1302_sec_add, ds1302_time[5]); // sec
		eeprom_write(5,buffer[5]);
	ds1302_write(ds1302_control_add, 0x80); // open write protection
}

#endif

/*******************************************
Function name: ds1302_read_time
Function: read the DS1302's current time
Parameters:
Return Value: None
********************************************/
// void ds1302_read_time (unsigned char set_time[])
// {
	
	// set_time[0] = ds1302_read(ds1302_year_add);  // years
	// set_time[1] = ds1302_read(ds1302_month_add); // month
	// set_time[2] = ds1302_read(ds1302_date_add); // day
	// //set_time[6] = ds1302_read(ds1302_day_add); // weeks
	// set_time[3] = ds1302_read(ds1302_hr_add); // time
	// set_time[4] = ds1302_read(ds1302_min_add); // min
	// set_time[5] = ds1302_read(ds1302_sec_add); // sec
	// }

void bcd2ascii(byte BCD,byte ptasc[])
{
	ptasc[0]= (BCD>>4) | 0x30; // convert ten
	ptasc[1] = BCD & 0x0F | 0x30;  // convert bits
}


// #if 0
/*******************************************
Function name: ds1302_display_time
Feature: 12864 shows the current time (line 1 format: year - month - day week; 2 line format: time - minutes - seconds)
Parameters: time [] - array of time
Return Value: None
********************************************/
// void ds1302_display_time(byte set_time[])
// {
	// byte asc[2];
	// byte line1[11] = {0,0 ,'-', 0,0 ,'-', 0,0, '', 0, '\ 0'}; // display an array of characters on line 1
	// byte line2[9] = {0,0 ,':', 0,0 ,':', 0,0, '\ 0'};  // display an array of characters in line 2

	// bcd2ascii (time[3], asc); // time
	// line2[0] = asc[0];
	// line2[1] = asc[1];
	// bcd2ascii (time[4], asc); // min
	// line2[3] = asc[0];
	// line2[4] = asc[1];
	// bcd2ascii (time[5], asc); // sec
	// line2[6] = asc[0];
	// line2[7] = asc[1];

	// bcd2ascii (time[0], asc); // for the assignment in line 1
	// line1[0] = asc[0];
	// line1[1] = asc[1];
	// bcd2ascii (time[1], asc); // for the assignment on line 1
	// line1[3] = asc[0];
	// line1[4] = asc[1];
	// bcd2ascii (time[2], asc); // for the assignment on line 1
	// line1[6] = asc[0];
	// line1[7] = asc[1];
	// bcd2ascii (time[6], asc); // line 1 week for the first assignment
	// line1[9] = asc[1];

// #if 0
	// Char_Set_XY (1,0, "20"); // line 1 position from the 1st show, will appear as 2007 2007 in the form of
	// Char_Set_XY (2,0, line1);
	// Char_Set_XY (2,1, line2); // line 2 from the first two position display * /
// #endif
// }
// #endif

// void ds1302_display_time(unsigned char set_time[])
// {
	// unsigned char asc[2];
	// unsigned char line1[11] = {0,0 ,'-', 0,0 ,'-', 0,0, ' ', '\0'}; // display an array of characters on line 1
	// unsigned char line2[9] = {0,0 ,':', 0,0 ,':', 0,0, '\0'};  // display an array of characters in line 2
	// int i=0;

	// // for(i=0; i<10; i++)
	// // {
	
	// bcd2ascii (set_time[3], asc); // time
	// line2[0] = asc[0];
	// eeprom_write(0,eeprom[0]);
	// line2[1] = asc[1];
	// eeprom_write(1,eeprom[1]);
	// bcd2ascii (set_time[4], asc); // min
	// line2[3] = asc[0];
	// line2[4] = asc[1];
	
	// bcd2ascii (set_time[5], asc); // sec
	// line2[6] = asc[0];
	// line2[7] = asc[1];

	// bcd2ascii (set_time[0], asc); // for the assignment in line 1
	// line1[0] = asc[0];
	// line1[1] = asc[1];
	
	// bcd2ascii (set_time[1], asc); // for the assignment on line 1
	// line1[3] = asc[0];
	// line1[4] = asc[1];
	
	// bcd2ascii (set_time[2], asc); // for the assignment on line 1
	// line1[6] = asc[0];
	// line1[7] = asc[1];
	
	// bcd2ascii (set_time[6], asc); // line 1 week for the first assignment
	// line1[9] = asc[1];

	// // usart0_format_puts("20%s %s\r\n", line1, line2);

	// initGlcd();
	// _delay_ms(1);
	// initGlcd();
	// _delay_ms(1);

	// for(i=0; i<10; i++)
	// {
		// putStringInGlcdAtPage(PAGE1, "      Welcome to  ");
		// putStringInGlcdAtPage(PAGE2,"    EOD Data Logger");
			// sprintf(g_glcdBuf,"20%s %s",line1,line2);
		// putStringInGlcdAtPage(PAGE4,g_glcdBuf);
		// putStringInGlcdAtPage(PAGE5, "   ver.1.01 Activated");
		// putStringInGlcdAtPage(PAGE3, "                   ");
		// putStringInGlcdAtPage(PAGE6, "                   ");
		// putStringInGlcdAtPage(PAGE7, "  anyang university");
		// putStringInGlcdAtPage(PAGE8, "     fin fo tech  ");
		// _delay_ms(50);
		// resetGlcd();
		// g_glcdBuf[5]++;
		// _delay_ms(1000);
	// }
	
// /*
	// else{
	// putStringInGlcdAtPage(PAGE1, ">>Data ");
	// putStringInGlcdAtPage(PAGE2, " ");
	// putStringInGlcdAtPage(PAGE3, ">>Setting ");
	// putStringInGlcdAtPage(PAGE4, " ");
	// putStringInGlcdAtPage(PAGE5, ">>File ");
	// putStringInGlcdAtPage(PAGE6, " ");
	// putStringInGlcdAtPage(PAGE7, ">>Information");
	// putStringInGlcdAtPage(PAGE8, " ");
	// _delay_ms(500);

	// }
  // }*/
// }

void delay_us(u16 us)
{
	u16 i;
	for( i=0;i<us;i++)
		asm("nop");
}

void displayDs1302ReadTime()
{
	unsigned char temp;
	temp = ds1302_read(ds1302_year_add);
	sprintf(g_glcdBuf, "Date=%d", (((0xF0&temp)>>4)*10)+(0x0F&temp));
	
	temp = ds1302_read(ds1302_month_add);
	sprintf(g_glcdBuf, "%s/%d", g_glcdBuf, (((0xF0&temp)>>4)*10)+(0x0F&temp));

	temp = ds1302_read(ds1302_date_add);
	sprintf(g_glcdBuf, "%s/%d,", g_glcdBuf, (((0xF0&temp)>>4)*10)+(0x0F&temp));

	temp = ds1302_read(ds1302_hr_add);
	sprintf(g_glcdBuf, "%s%d", g_glcdBuf, (((0xF0&temp)>>4)*10)+(0x0F&temp));
	
	temp = ds1302_read(ds1302_min_add);
	sprintf(g_glcdBuf, "%s:%d", g_glcdBuf, (((0xF0&temp)>>4)*10)+(0x0F&temp));

	temp = ds1302_read(ds1302_sec_add);
	sprintf(g_glcdBuf, "%s:%d", g_glcdBuf, (((0xF0&temp)>>4)*10)+(0x0F&temp));

	putStringInGlcdAtPage(PAGE1, g_glcdBuf);
}

/* Test Sample codes */
// void main()
// {
	// unsigned char get_time [7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // array to save the current time
	// unsigned char cur_time [7] = {0x30, 0x27, 0x14, 0x24, 0x07, 0x05, 0x14}; // seconds, minutes, hours, days, months, weeks, years

	// unsigned char asc[2];
	// unsigned char line1[11] = {0,0 ,'-', 0,0 ,'-', 0,0, ' ', '\0'}; // display an array of characters on line 1
	// unsigned char line2[9] = {0,0 ,':', 0,0 ,':', 0,0, '\0'};  // display an array of characters in line 2


	// ds1302_set_time(cur_time); //½Ã°£À» ¼¼ÆÃÇØÁÜ.
	// ds1302_read_time(get_time); //½Ã°£À» ÀÐ¾î¿Â´Ù.
	
	// bcd2ascii (get_time[3], asc); // time
	// line2[0] = asc[0];
	// line2[1] = asc[1];
	// bcd2ascii (get_time[4], asc); // min
	// line2[3] = asc[0];
	// line2[4] = asc[1];
	// bcd2ascii (get_time[5], asc); // sec
	// line2[6] = asc[0];
	// line2[7] = asc[1];
	// bcd2ascii (get_time[0], asc); // for the assignment in line 1
	// line1[0] = asc[0];
	// line1[1] = asc[1];
	// bcd2ascii (get_time[1], asc); // for the assignment on line 1
	// line1[3] = asc[0];
	// line1[4] = asc[1];
	// bcd2ascii (get_time[2], asc); // for the assignment on line 1
	// line1[6] = asc[0];
	// line1[7] = asc[1];
	// bcd2ascii (get_time[6], asc); // line 1 week for the first assignment
	// line1[9] = asc[1];
	

	// while(1)
	// {
		// ds1302_read_time (set_time[]);
		// ds1302_display_time(set_time[]);
	// }
// }
