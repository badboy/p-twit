#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>
#include <iostream>
#include <stdexcept> // runtime_error
#include <ctime>

#include "tinyxml.h"
#include "tinystr.h"

using namespace std;

const string shortdays[] = {
	"Sun",
	"Mon",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};

const string months[] = {
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

const string shortmonths[] = {
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

int day_to_i(string dayname)
{
	int i;
	for(i=6; i>-1;--i)
	{
		if(dayname == shortdays[i]) break;
	}

	return i;
}

int month_to_i(string monthname)
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
	int timediff;
	sscanf(datestring, "%s %s %d %d:%d:%d %c%d %d", dayname, month, &day, &hour, &minute, &second, &plusminus, &timediff, &year);
	int month_i = month_to_i(month);
	int day_i = day_to_i(dayname);

	struct tm *now = new struct tm;

	now->tm_sec = second;
	now->tm_min = minute;
	now->tm_hour = hour;
	now->tm_mday = day;
	now->tm_mon = month_i;
	now->tm_year = year - 1900;
	now->tm_wday = day_i;
	now->tm_yday = 33;
	char date2[255];
	sprintf(date2, "%d.%d.%d %d:%d:%s%d", day, month_i, year, hour, minute, (second<10?"0":""),second);

	return now;
}

//extern TiXmlString::Rep TiXmlString::nullrep_;

// Den Inhalt eines XML-Elements auslesen, das nur Text enthält
const char* textValue(TiXmlElement* e)
{
    TiXmlNode* first = e->FirstChild( );
    if ( first != 0 &&
         first == e->LastChild( ) &&
         first->Type( ) == TiXmlNode::TEXT )
    {
        // Das Element e hat ein einzelnes Kind vom Typ TEXT;
        // Inhalt zurückgeben.
        return first->Value( );
    } else {
            return NULL; //throw runtime_error(string("falsches Element ") + e->Value( ));
    }
}

int sub(string pattern, string replacement, string& input)
{
	size_t found = input.find(pattern);
	size_t size = input.size();
	if(found < size)
	{
		input.replace(found, pattern.size(), replacement);
		return 1;
	}
	else return 0;
}

void ErrorExit(const char *fmt, ...)
{
	va_list list;
	char msg[256];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	printf(msg);
	exit(1);
}

typedef struct _TwitterUser
{
	int id;
	string name;
	string screen_name;
	string avatar;
	string homepage;
} TwitterUser;

typedef struct _StatusEntry
{
	struct tm *time;
	int id;
	string text;
	string source;
	int in_reply_to_status_id;
	int in_reply_to_user_id;
	TwitterUser user;
} StatusEntry;

StatusEntry *parseTimeline(const char *filename, int *count)
{
	TiXmlDocument doc(filename);
	if(!doc.LoadFile()) { printf("Can't load file '%s'!\n", filename); return NULL; }
//	else printf("File '%s' successfully loaded!\n", filename);

	TiXmlElement* root = doc.RootElement();
	if(!root)
	{
		printf("Fehlendes root-Element!\n");
		return NULL;
	}
	if (strcmp(root->Value( ), "statuses") != 0)
	{
		printf("falsche Wurzel: %s\n", root->Value());
		return NULL;
	}

	StatusEntry *entries = new StatusEntry[20];

	int i = 0;
	for(TiXmlElement* elem = root->FirstChildElement( ); elem; elem = elem->NextSiblingElement())
	{
		if(strcmp(elem->Value(), "status") != 0) continue;

		// get message information
		TiXmlElement *_date = elem->FirstChildElement("created_at");
		TiXmlElement *_id = elem->FirstChildElement("id");
		TiXmlElement *_text = elem->FirstChildElement("text");
		TiXmlElement *_source = elem->FirstChildElement("source");

		if(!_date || !_id || !_text || !_source)
		{
			printf("getting message info failed!\n");
			return NULL;
		}

		// get user information
		TiXmlElement *user = elem->FirstChildElement("user");
		if(!user)
		{
			printf("user element not found!\n");
			return NULL;
		}

		TiXmlElement *_user_id = user->FirstChildElement("id");
		TiXmlElement *_name = user->FirstChildElement("name");
		TiXmlElement *_screen_name = user->FirstChildElement("screen_name");
		TiXmlElement *_ava = user->FirstChildElement("profile_image_url");

		if(!_user_id || !_name || !_screen_name || !_ava)
		{
			printf("getting user info failed!\n");
			return NULL;
		}

		string date = textValue(_date);
		struct tm *tm_now = makedate(date.c_str());

		int id = atoi(textValue(_id));
		string name = textValue(_name);
		string text = textValue(_text);
		string ava = textValue(_ava);
		string source = textValue(_source);

		int user_id = atoi(textValue(_user_id));
		string user_screen_name = textValue(_screen_name);
		string user_name = textValue(_name);
		string user_ava = textValue(_ava);

		sub("&gt;", ">", text);
		sub("&lt;", "<", text);

		entries[i].time = tm_now;
		entries[i].id = id;
		entries[i].text = text;
		entries[i].source = source;
		entries[i].in_reply_to_status_id = 0;
		entries[i].in_reply_to_user_id = 0;

		entries[i].user.id = user_id;
		entries[i].user.screen_name = user_screen_name;
		entries[i].user.name = user_name;
		entries[i].user.avatar = user_ava;

		if(i == 19) break;
		++i;
	}
	*count = i+1;
	return entries;
}

const char *filename = "../../friends_timeline.xml";
int main(int argc, char** argv)
{
	int count;
	StatusEntry *entries = parseTimeline(filename, &count);

	cout << count << " Einträge gefunden!" << endl;

	int i;
	for(i=0;i<count;i++)
	{
		char date[256];
		strftime (date, 256, "%x %X", entries[i].time);
		cout << entries[i].user.name << ": " << entries[i].text << endl;
	}
return 0;
}


