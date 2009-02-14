#include <string.h>

#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"

extern "C"
{
#include "tri/triLog.h"
}

#include "init.h"
#include "helpers.h"
#include "datehelpers.h"
#include "timeline.h"

int parseTimeline(const char *content, StatusEntry *entries, int max)
{
	if(strlen(content) <= 0) return 0;
	TiXmlDocument doc;
	if(!doc.Parse(content))
	{
		triLogError("Can't parse XML Code !\n");
		return 0;
	}

	TiXmlElement* root = doc.RootElement();
	if(!root)
	{
		triLogError("no root-element!\n");
		return 0;
	}
	if (strcmp(root->Value( ), "statuses") != 0)
	{
		triLogError("wrong root: %s\n", root->Value());
		return 0;
	}

	//memset(entries, 0, max * sizeof(StatusEntry));
	//StatusEntry *entries = new StatusEntry[max];

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
			triLogError("getting message info failed!\n");
			return 0;
		}

		// get user information
		TiXmlElement *user = elem->FirstChildElement("user");
		if(!user)
		{
			triLogError("user element not found!\n");
			return 0;
		}

		TiXmlElement *_user_id = user->FirstChildElement("id");
		TiXmlElement *_name = user->FirstChildElement("name");
		TiXmlElement *_screen_name = user->FirstChildElement("screen_name");
		TiXmlElement *_ava = user->FirstChildElement("profile_image_url");

		if(!_user_id || !_name || !_screen_name || !_ava)
		{
			triLogError("getting user info failed!\n");
			return 0;
		}

		std::string date = textValue(_date);
		struct tm *tm_now = makedate(date.c_str());

		int id = atoi(textValue(_id));
		std::string name = textValue(_name);
		std::string text = textValue(_text);
		std::string ava = textValue(_ava);
		std::string source = textValue(_source);

		int user_id = atoi(textValue(_user_id));
		std::string user_screen_name = textValue(_screen_name);
		std::string user_name = textValue(_name);
		std::string user_ava = textValue(_ava);

		str_sub("&gt;", ">", text);
		str_sub("&lt;", "<", text);

		time_t ts = mktime(tm_now);
		entries[i].time = tm_now;
		entries[i].time_text = gen_time_text(ts);
		entries[i].timestamp = ts;
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
	return i+1;
}
