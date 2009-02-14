#include <string>
#include "includes.h"

const std::string shortdays[] = {
	"Sun",
	"Mon",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};

const std::string months[] = {
	"January",
	"February",
	"March",
	"April", 
	"May", 
	"June", 
	"July", 
	"August",
	"September",
	"October",
	"November",
	"December"
};

const std::string shortmonths[] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr", 
	"May", 
	"Jun", 
	"Jul", 
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

int day_to_i(std::string dayname)
{
	int i;
	for(i=6; i>-1;--i)
	{
		if(dayname == shortdays[i]) break;
	}

	return i;
}

int month_to_i(std::string monthname)
{
	int i;
	for(i=11; i>-1;--i)
	{
		if(monthname == shortmonths[i]) break;
	}

	return i;
}

struct tm *makedate(const char *datestring)
{
	char month[20], dayname[20];
	int day, hour, minute, second, year;
	char plusminus;
	int _timediff;
	sscanf(datestring, "%s %s %d %d:%d:%d %c%d %d", dayname, month, &day, &hour, &minute, &second, &plusminus, &_timediff, &year);
	int month_i = month_to_i(month);
	int day_i = day_to_i(dayname);

	struct tm *now = new struct tm;

	now->tm_sec = second;
	now->tm_min = minute;
	now->tm_hour = hour;// + timediff;
	now->tm_mday = day;
	now->tm_mon = month_i;
	now->tm_year = year - 1900;
	now->tm_wday = day_i;

	return now;
}

int timediff=0;
char clockformat[255];

void set_clockformat(int dateformat, int timeformat)
{
	std::string date, time;
	switch(dateformat)
	{
		case 0: date = "%Y/%m/%d"; break;
		case 1: date = "%m/%d/%Y"; break;
		case 2: date = "%d/%m/%Y"; break;
		default: date = "%x"; break;		
	}
#define PSP_SYSTEMPARAM_TIME_FORMAT_24HR	0
#define PSP_SYSTEMPARAM_TIME_FORMAT_12HR	1
	switch(timeformat)
	{
		case 0: time = "%H"; break;
		case 1: time = "%I %p"; break;
		default: time = "%X"; break;	
	}

	sprintf(clockformat, "%s - %s", date.c_str(), time.c_str());
}

std::string gen_time_text(time_t time)
{
	triLogPrint("in gen_time_text (timediff: %d)\n", timediff);
	char date[255];
	clock_t now = sceKernelLibcTime(NULL);

	time_t exist = now - time;
	triLogPrint("now: %d, time: %d, diff: %d\n", now, time, exist);

	int day = exist / 60 / 60 / 24 % 7;
	int hour = exist / 60 / 60 % 24;
	int min = exist / 60 % 60;
	int sec = exist % 60;

	triLogPrint("day:%d, hour:%d, min:%d, sec:%d, exist:%d\n", day, hour, min, sec, exist);

	if(day > 1)
	{
		time += timediff;
		struct tm * timeinfo = localtime(&time);
		strftime(date, 200, "%x - %X", timeinfo);
/*		sprintf(date, "%d/%d/%d - %d:%s%d", timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_year+1900, timeinfo->tm_hour, (timeinfo->tm_min<10?"0":""), timeinfo->tm_min);*/
	}
	else if(day == 1) sprintf(date, "about 1 day ago");
	else if(hour > 0) sprintf(date, "about %d hour%s ago", hour, hour==1?"":"s");
	else if(min > 1) sprintf(date, "%d minute%s ago", min, (min==1?"":"s"));
	else if(sec > 0)
	{
		if(sec < 60) sprintf(date, "less than one minute ago");
		if(sec < 50) sprintf(date, "less than 50 seconds ago");
		if(sec < 40) sprintf(date, "less than 40 seconds ago");
		if(sec < 30) sprintf(date, "less than 30 seconds ago");
		if(sec < 20) sprintf(date, "less than 20 seconds ago");
		if(sec < 10) sprintf(date, "less than 10 seconds ago");
		if(sec < 5) sprintf(date, "less than 5 seconds ago");
	}
	else
	{
		time += timediff;
		struct tm * timeinfo = localtime(&time);
		strftime(date, 200, "%x - %X", timeinfo);
		/*sprintf(date, "%d/%d/%d - %d:%s%d", timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_year+1900, timeinfo->tm_hour, (timeinfo->tm_min<10?"0":""), timeinfo->tm_min);*/
	}

	return std::string(date);
}
