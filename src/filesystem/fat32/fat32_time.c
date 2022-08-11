void updateDateFromDS1302(dirDateInfo *date)
{
	unsigned char temp;
	temp = ds1302_read(ds1302_year_add);
	(*date).year = ((((DS1302_TENTH_MASK&temp)>>DS1302_TENTH_OFFSET)*10)+(DS1302_ONEST_MASK&temp)+DS1302_THOUSAND);
	temp = ds1302_read(ds1302_month_add);
	(*date).month = (((DS1302_TENTH_MASK&temp)>>DS1302_TENTH_OFFSET)*10)+(DS1302_ONEST_MASK&temp);
	temp = ds1302_read(ds1302_date_add);
	(*date).day = (((DS1302_TENTH_MASK&temp)>>DS1302_TENTH_OFFSET)*10)+(DS1302_ONEST_MASK&temp);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void updateTimeFromDS1302(dirTimeInfo *time)
{
	unsigned char temp;
	temp = ds1302_read(ds1302_hr_add);
	(*time).hour = (((DS1302_TENTH_MASK&temp)>>DS1302_TENTH_OFFSET)*10)+(DS1302_ONEST_MASK&temp);
	temp = ds1302_read(ds1302_min_add);
	(*time).minute = (((DS1302_TENTH_MASK&temp)>>DS1302_TENTH_OFFSET)*10)+(DS1302_ONEST_MASK&temp);
	temp = ds1302_read(ds1302_sec_add);
	(*time).second = (((DS1302_TENTH_MASK&temp)>>DS1302_TENTH_OFFSET)*10)+(DS1302_ONEST_MASK&temp);
}
