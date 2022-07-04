#ifndef _FAT32_TIME_H_
#define _FAT32_TIME_H_
/*
Pointer variable named entry must indicate entry+time offset(are create, access and write) bit of dir entry.
If any pointer is DIR_name[0], entry in dirTimeOnfoParseFromDirectoryEntry have +DIR_CREATION_DATE_OFFSET, DIR_LAST_ACCESS_DATE_OFFSET or DIR_WRITE_DATE_OFFSET
Normally, used structures are differently used.
When make directory or file(archive), dirDateInfo and dirTimeInfo are writed in create date and time.
Also write and last access date time are have same value.

But, directory and file are varied, these are write at last access and write section.
*/
#define DS1302_THOUSAND	2000
#define DS1302_TENTH_OFFSET	4
#define DS1302_TENTH_MASK	0xF0
#define DS1302_ONEST_MASK	0x0F

#endif
