#ifndef TWITTER_H_
#define TWITTER_H_

#include <string>
extern std::string lastResponseText;
int TwitterUpdateStatus(Config *config, std::string status, int status_id=0, int user_id=0);
std::string TwitterTimeline(Config *config);
std::string TwitterFriendsTimeline(Config *config);

#endif
