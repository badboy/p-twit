#ifndef TIMELINE_H_
#define TIMELINE_H_

#include <string>

typedef struct _TwitterUser
{
	int id;
	std::string name;
	std::string screen_name;
	std::string avatar;
	std::string homepage;
} TwitterUser;

typedef struct _StatusEntry
{
	struct tm *time;
	time_t timestamp;
	std::string time_text;
	int id;
	std::string text;
	std::string source;
	int in_reply_to_status_id;
	int in_reply_to_user_id;
	TwitterUser user;
} StatusEntry;

int parseTimeline(const char *content, StatusEntry *entries, int max=20);

#endif
