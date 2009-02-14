#include <string>
#include "includes.h"
#include "UriCodec.h"
#include "netextended.h"
#include "twitter.h"

const std::string TwitterHost = "twitter.com";
const std::string TwitterUpdateSite = "/statuses/update.xml";
const std::string TwitterTimelineSite = "/statuses/user_timeline/";
const std::string TwitterFriendTimelineSite = "/statuses/friends_timeline.xml";

std::string lastResponseText;

int TwitterUpdateStatus(Config *config, std::string status, int status_id, int user_id)
{
	lastResponseText.clear();
	if(config == NULL || status.size() <= 0) return 0;
	if(status.size() > 140) return 0;
	if(!triNetSwitchStatus()) return 0;
	if(!triNetIsConnected()) return 0;

	status = UriEncode(status);

	std::string status_update = "status="+status;
	if(status_id > 0 && user_id > 0)
	{
		status_update += "&in_reply_to_status_id=" + status_id;
		status_update += "&in_reply_to_user_id=" + user_id;
	}
	status_update += "&source=ptwit";
	HTTPRequest req = { TwitterHost, TwitterUpdateSite, 2, status_update, "psptwitter v0.0.2 <BadBoy_>", config->base64_code};
	NetGetUrlExtended(&req);
	lastResponseText = req.responseText;
	if(req.responseCode != 200)
	{
		triLogError("responseCode: %d\n", req.responseCode);
		triLogError("responseHeader:\n%s\n", req.responseHeader.c_str());
		return 0;
	}

	return 1;
}

std::string TwitterTimeline(Config *config)
{
	return "not implemented";
}

std::string TwitterFriendsTimeline(Config *config)
{
	lastResponseText.clear();
	if(config == NULL) return "";
	if(!triNetSwitchStatus()) return "";
	if(!triNetIsConnected()) return "";

	HTTPRequest req = { TwitterHost, TwitterFriendTimelineSite, 0, "", "psptwitter v0.0.1 <BadBoy_>", config->base64_code};
	NetGetUrlExtended(&req);
	lastResponseText = req.responseText;
	if(req.responseCode != 200)
	{
		triLogError("responseCode: %d\n", req.responseCode);
		triLogError("responseHeader:\n%s\n", req.responseHeader.c_str());
		return "";
	}

	return req.body;
}
