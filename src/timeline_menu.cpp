#include <cmath>
#include "includes.h"
#include "options_menu.h"
#include "init.h"
#include "osk.h"
#include "CConfig.h"

#include "twitter.h"
#include "helpers.h"
#include "timeline.h"

#define MAX_PER_PAGE (3.f)

#include "test.h"

int sendReply(Config *config, std::string tweet, int status_id, int user_id)
{
	triClear(0);
	triDrawImage2( 0.f, 0.f, background );
	triDrawImage2( 250.f, 10.f, twitterlogo );
	triDrawImage2( 260.f, 65.f, arrow );
	triDrawRect( 30.f, 76.f, 340.f, 23.f, 0xFFFFFFFF );
	triFontActivate(verdana);
	triFontPrint(verdana, 35, 80, 0xFF000000, "sending reply...");
	triImageNoTint();
	triSwapbuffers();

	if(!triNetIsConnected())
		initNet(config->wlan_connection);
	return TwitterUpdateStatus(config, tweet);
}

int getTimeline(Config *config, StatusEntry *entries)
{
		triClear(0);
		triDrawImage2( 0.f, 0.f, background );
		triDrawImage2( 250.f, 10.f, twitterlogo );
		triDrawImage2( 260.f, 65.f, arrow );
		triDrawRect( 30.f, 76.f, 340.f, 23.f, 0xFFFFFFFF );
		triFontActivate(verdana);
		triFontPrint(verdana, 35, 80, 0xFF000000, "receiving timeline...");
		triImageNoTint();
		triSwapbuffers();

		if(!triNetIsConnected()) 
		{
			if(!initNet(config->wlan_connection)) return 0;
		}
		std::string timeline = TwitterFriendsTimeline(config);
		if(timeline.size() <= 0)
			return 0;
		else
			return parseTimeline(timeline.c_str(), entries);
}

int timeLineMenu(Config *config, int forcereload)
{
	lastResponseText.clear();
	static StatusEntry entries[20];
	static int max_entries;
	static clock_t last_get;

	unsigned int delay_time = 2000000;
	clock_t now = sceKernelLibcClock();
	triInputUpdate();
	if(triInputPressed(PSP_CTRL_RTRIGGER))
	{
		triLogPrint("rtrigger is pressed -> forcereload\n");
		forcereload = 1;
	}

	if(forcereload || last_get <= 0 || (now - last_get) < delay_time)
	{

		if(!triNetSwitchStatus())
			return 0;

		last_get = now;
		triLogPrint("getting timeline...\n");

		max_entries = getTimeline(config, entries);
		triLogPrint("\nreceived %d entries\n", max_entries);
		if(max_entries <= 0) { last_get = 0; return 0; }
	}
	int timeline_isrunning = 1;

	triFloat nick_pos_x = 16.f;
	triFloat nick_pos[3] = { 70.f, 139.f, 209.f };

	int msg_pos = 0, msg_y_adj = 15;
	int offset = 0;
	triFloat scroll_y = 62.f, scroll_diff = 172.0 / (max_entries-1);

	triLogPrint("scroll_diff: %.2f\n", scroll_diff);
	std::string downcase_user_nick = config->user_nick;
	str_tolower(downcase_user_nick);


	while (timeline_isrunning)
	{
		triInputUpdate();

		if(triInputPressed(PSP_CTRL_TRIANGLE))
		{
			triLogPrint("leave timeline menu\n");
			timeline_isrunning = 0;
			continue;
		}

		if(triInputPressed(PSP_CTRL_CROSS))
		{
			const char *reply = SonyOSK(140, "Reply to tweet", ("@"+entries[msg_pos+offset].user.name+" ").c_str() , background);
			triLogPrint("reply: %s\n", reply);


			int user_id = entries[msg_pos+offset].user.id;
			int status_id = entries[msg_pos+offset].id;

			if(reply && strlen(reply) > 0)
			{
				if(sendReply(config, reply, status_id, user_id))
				{
					max_entries = getTimeline(config, entries);
					scroll_diff = 172.0 / (max_entries-1);
					continue;
				}
				else
				{
					triClear(0);
					triDrawImage2( 0.f, 0.f, background );
					triDrawImage2( 250.f, 10.f, twitterlogo );
					triDrawImage2( 260.f, 65.f, arrow );
					triDrawRect( 30.f, 76.f, 340.f, 23.f, 0xFFFFFFFF );
					triFontActivate(verdana);
					triFontPrint(verdana, 35, 80, 0xFF000000, "error sending reply");
					triImageNoTint();
					triSwapbuffers();
					sceKernelDelayThread(2*1000*1000);
					continue;
				}
			}
		}

		if(triInputPressed(PSP_CTRL_UP))
		{
			if(msg_pos == 0 && offset > 0) { --offset; scroll_y -= scroll_diff; }
			else if(msg_pos > 0) { --msg_pos; scroll_y -= scroll_diff; }
			triLogPrint("timeline msg_pos: %d, scroll_y: %.2f, offset: %d\n", msg_pos, scroll_y, offset);
		}
		if(triInputPressed(PSP_CTRL_DOWN)) 
		{
			if(msg_pos == 2 && offset < (max_entries-3)) { ++offset; scroll_y += scroll_diff; }
			else if(msg_pos < 2) { ++msg_pos; scroll_y += scroll_diff; }
			triLogPrint("timeline msg_pos: %d, scroll_y: %.2f, offset: %d\n", msg_pos, scroll_y, offset);
		}

		if(triInputPressed(PSP_CTRL_LTRIGGER))
		{
			triImage* snap = triImageGet( 0, 0, 480, 272 );
			triImageSavePng( "timeline_screenshot.png", snap, 0 ); 
			triImageFree( snap );
			triLogPrint("Screenshot saved to timeline_screenshot.png\n");
		}

		if(triInputPressed(PSP_CTRL_RTRIGGER))
		{
			triNetDisconnect();
		}

		triClear(0);

		triDrawImage2( 0.f, 0.f, background );
		triDrawImage2( 250.f, 10.f, twitterlogo );

		// scrollbalken
		triDrawImage2( 458.f, 60.f, scroll );
		triDrawImage2( 460.f, scroll_y, scroller );

		// erste nachricht
		triDrawImage2( 12.f, 65.f, message_box );
		// zweite "
		triDrawImage2( 12.f, 134.f, message_box );
		// dritte "
		triDrawImage2( 12.f, 204.f, message_box );

		std::string tmp = entries[msg_pos+offset].user.name;
		str_tolower(tmp);
		if(tmp != downcase_user_nick)//if(entries[msg_pos+offset].user.name != config->user_nick)
			triDrawImage2( 407.f, nick_pos[msg_pos]-3, reply_cross );

		triDrawImage2( 30.f, 10.f, arrow );
		triDrawRect( 10.f, 21.f, 180.f, 23.f, 0xFFFFFFFF );
		triFontActivate(verdana);
		triFontPrint(verdana, 50, 25, BLACK, "Timeline");

		triFontActivate(verdanaSmall);
		triFontPrintf(verdanaSmall, nick_pos_x, nick_pos[0], BLACK, "%s:", entries[0+offset].user.name.c_str());
//		triFontPrintf(verdanaSmall, nick_pos_x+5, nick_pos[0]+msg_y_adj, BLACK, split("abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ").c_str());
		triFontPrintf(verdanaSmall, nick_pos_x+5, nick_pos[0]+msg_y_adj, BLACK, "%s", split(entries[0+offset].text).c_str());

		if((1+offset) < max_entries)
		{
			triFontPrintf(verdanaSmall, nick_pos_x, nick_pos[1], BLACK, "%s:", entries[1+offset].user.name.c_str());
			triFontPrintf(verdanaSmall, nick_pos_x+5, nick_pos[1]+msg_y_adj, BLACK, "%s", split(entries[1+offset].text).c_str());
		}


		if((2+offset) < max_entries)
		{
			triFontPrintf(verdanaSmall, nick_pos_x, nick_pos[2], BLACK, "%s:", entries[2+offset].user.name.c_str());
			triFontPrintf(verdanaSmall, nick_pos_x+5, nick_pos[2]+msg_y_adj, BLACK, "%s", split(entries[2+offset].text).c_str());
		}

		triFontActivate(verdanaSmaller);
		triFontPrint(verdanaSmaller, 180, nick_pos[0], BLACK, entries[0+offset].time_text.c_str());
		triFontPrint(verdanaSmaller, 180, nick_pos[1], BLACK, entries[1+offset].time_text.c_str());
		triFontPrint(verdanaSmaller, 180, nick_pos[2], BLACK, entries[2+offset].time_text.c_str());

		if(msg_pos != 0) triDrawImage2( 12.f, 65.f, message_box_layer );
		if(msg_pos != 1) triDrawImage2( 12.f, 134.f, message_box_layer );
		if(msg_pos != 2) triDrawImage2( 12.f, 204.f, message_box_layer );

		triImageNoTint();
		triSwapbuffers();
	}

	triClear( 0 );
	triSwapbuffers();
	sceKernelDelayThread(20000);
	return 1;
}
