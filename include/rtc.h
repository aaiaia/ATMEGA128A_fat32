#ifndef _RTC_DS1302_H_
#define _RTC_DS1302_H_

// ds1302
#define DS1302_DDR			DDRG
#define DS1302_PORT			PORTG
#define DS1302_PIN			PING

#define DS1302_RST_PIN_NO	PG0
#define DS1302_IO_PIN_NO		PG1
#define DS1302_SCLK_PIN_NO	PG2

#define set_ds1302_rst_ddr()  DS1302_DDR|=1<<DS1302_RST_PIN_NO        // reset port set to output
#define set_ds1302_rst()      DS1302_PORT|=1<<DS1302_RST_PIN_NO       // reset terminal set
#define clr_ds1302_rst()      DS1302_PORT&=~(1<<DS1302_RST_PIN_NO)    // reset port cleared

#define set_ds1302_io_ddr()   DS1302_DDR|=1<<DS1302_IO_PIN_NO         // output data terminal set
#define set_ds1302_io()       DS1302_PORT|=1<<DS1302_IO_PIN_NO        // data terminal set
#define clr_ds1302_io()       DS1302_PORT&=~(1<<DS1302_IO_PIN_NO)     // data terminal cleared
#define clr_ds1302_io_ddr()   DS1302_DDR&=~(1<<DS1302_IO_PIN_NO)      // data is input side
#define in_ds1302_io()        DS1302_PIN&(1<<DS1302_IO_PIN_NO)        // data terminal input data

#define set_ds1302_sclk_ddr() DS1302_DDR|=1<<DS1302_SCLK_PIN_NO       // clock output is set to end
#define set_ds1302_sclk()     DS1302_PORT|=1<<DS1302_SCLK_PIN_NO      // set the clock terminal
#define clr_ds1302_sclk()     DS1302_PORT&=~(1<<DS1302_SCLK_PIN_NO)   //

#define ds1302_sec_add		0x80 // second data address
#define ds1302_min_add		0x82 // address of data points
#define ds1302_hr_add		0x84 // when the data address
#define ds1302_date_add		0x86 // address of daily data
#define ds1302_month_add	0x88 // address on the data
#define ds1302_day_add		0x8a // week data address
#define ds1302_year_add		0x8c // address data for the year
#define ds1302_control_add	0x8e // control data address
#define ds1302_charger_add	0x90
#define ds1302_clkburst_add	0xbe
#define sbi(port, bit)				(port |= (1<<bit))
#define cbi(port, bit)				(port &= (~(1<<bit)))
#define inp(port, bit)				(port & (1<<bit))
#define CLI() cli()
#define SEI() sei()

typedef unsigned char		byte;
typedef unsigned char		u8;
typedef signed int			s16;
typedef unsigned int		u16;

void bsp_ds1302_gpio_init(void);
void ds1302_write(byte add , byte data);
byte ds1302_read(byte add);
byte check_ds1302(void);
void ds1302_set_time(byte set_time[]);
void ds1302_write_time(void);
void ds1302_read_time (byte set_time []);
void bcd2ascii(byte BCD,byte ptasc[]);
void delay_us(u16 us);
void ds1302_DataView(byte set_time[]);
void eeprom_store1(unsigned char line2);
void eeprom_store2(unsigned char line1);

#endif
