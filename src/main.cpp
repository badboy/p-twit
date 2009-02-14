#include "includes.h"
#include "init.h"
#include "osk.h"
#include "CConfig.h"

#include "twitter.h"
#include "options_menu.h"
#include "timeline_menu.h"
#include "helpers.h"
#include "netextended.h"

PSP_MODULE_INFO("p-twit", 0x0, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(20480);


int menu_pos(triFloat *cur, int direct)
{
	triFloat change = 33.f;
	triFloat min=131.f, max=230.f;

	if(direct) // up
	{
		if(*cur > min)
			*cur -= change;
	}
	else // down
	{
		if(*cur < max)
			*cur += change;
	}

	if(*cur == min) return 1;
	if(*cur == 164.f) return 2;
	if(*cur == 197.f) return 3;
	if(*cur == max) return 4;

return 0;
}

char draw_msg[512];

int main(int argc, char **argv)
{
	if(argc > 1 && (std::string(argv[1]) == "debug" || std::string(argv[1]) == "-d"))
		init(1);
	else
		init(0);

	Config config("config.xml");
	if(!config.is_loaded())
	{
		GraphicErrorExit(5000, "Can't load config! Exiting...");
	}

	if(triNetSwitchStatus())
	{
		triClear( TWITTER );
		triDrawImage2( 0.f, 0.f, background );
		triFontActivate(twitterfont);
		triFontPrint(twitterfont, 40, 130, TWITTER, "initializing network...");
		triImageNoTint();
		triSwapbuffers();

		if(!initNet(config.wlan_connection))
		{
			GraphicErrorExit(5000, "Can't init network! Exiting...");
		}
	}

	clock_t last_draw_start = 0;

	triFloat bird_y = 230.f;
	int menupos = 4;

	clock_t now;
	unsigned int delay_time = 2000000;

	std::string tweet;
	int forcereload = 0;

	triLogPrint("entering main while loop\n");

	while (isrunning)
	{
		triInputUpdate();
		if(triInputPressed(PSP_CTRL_UP))
		{
			menupos = menu_pos(&bird_y, 1);
			triLogPrint("bird_y: %.2f, menupos: %d\n", bird_y, menupos);
		}
		if(triInputPressed(PSP_CTRL_DOWN)) 
		{
			menupos = menu_pos(&bird_y, 0);
			triLogPrint("bird_y: %.2f, menupos: %d\n", bird_y, menupos);
		}
		if(triInputPressed(PSP_CTRL_LTRIGGER))
		{
			triImage* snap = triImageGet( 0, 0, 480, 272 );
			triImageSavePng( "screenshot.png", snap, 0 ); 
			triImageFree( snap );
			triLogPrint("Screenshot saved to screenshot.png\n");
		}
		if(triInputPressed(PSP_CTRL_CROSS))
		{
			switch(menupos)
			{
				case 1:
				{
					const char *_tweet = SonyOSK(140, "Enter your tweet", "", background);
					if(_tweet && strlen(_tweet) > 0)
					{
						tweet = _tweet;
					}
					break; // twit it
				}
				case 2: 
				{
					if(timeLineMenu(&config, forcereload) == 0)
					{
						if(lastResponseText.size() > 0)
							sprintf(draw_msg, "Error: %s", lastResponseText.c_str());
						else
							strcpy(draw_msg, "Error receiving timeline\0");
						last_draw_start = sceKernelLibcClock();
					}
					forcereload = 0;
					break; // timeline
				}
				case 3: 
				{
					switch(optionsMenu(&config))
					{
						case 1:
							strcpy(draw_msg, "Settings saved\0");
							forcereload = 1;
							last_draw_start = sceKernelLibcClock();
							break;
						case 2:
							strcpy(draw_msg, "Error saving settings\0");
							last_draw_start = sceKernelLibcClock();
							break;
					}
					break; // timeline
				}
				case 4: isrunning = 0; continue; break; // exit
				default: break;
			}
		}

		triClear(0);
//		sceGuFinish();
//		sceGuSync(0,0);

		triDrawImage2( 0.f, 0.f, background );
		triDrawImage2( 250.f, 10.f, twitterlogo );

		if(tweet.size() > 0)
		{
			if(!triNetSwitchStatus())
			{
				strcpy(draw_msg, "WLAN Switch is off\0");
				last_draw_start = sceKernelLibcClock();
			}
			else
			{
				triDrawImage2( 260.f, 65.f, arrow );
				triDrawRect( 30.f, 76.f, 340.f, 23.f, 0xFFFFFFFF );

				triFontActivate(verdana);
				triFontPrint(verdana, 35, 80, 0xFF000000, "Sending...");
				triImageNoTint();
				triSwapbuffers();

				if(!triNetIsConnected())
					initNet(config.wlan_connection);
				if(TwitterUpdateStatus(&config, tweet))
					strcpy(draw_msg, "Updated!");
				else
					sprintf(draw_msg, "Error: %s", lastResponseText.c_str());
				last_draw_start = sceKernelLibcClock();
			}

			tweet.clear();
		}

		triDrawImage2( 10.f, bird_y, bird ); // CURSOR

		if(!triNetSwitchStatus())
		{
			triDrawImage2( 30.f, 10.f, arrow );
			triDrawRect( 10.f, 21.f, 200.f, 23.f, 0xFFFFFFFF );

		}

		now = sceKernelLibcClock();
		if(last_draw_start != 0 && (now - last_draw_start) < delay_time)
		{
			triDrawImage2( 260.f, 65.f, arrow );
			triDrawRect( 30.f, 76.f, 340.f, 23.f, 0xFFFFFFFF );
			triFontActivate(verdana);
			triFontPrint(verdana, 35, 80, 0xFF000000, draw_msg);
		}
		else last_draw_start = 0;

		if(!triNetSwitchStatus())
		{
			triFontActivate(verdana);
			triFontPrint(verdana, 25, 25, 0xFF000000, "WLAN is off!");
		}

		triFontActivate(twitterfont);
		triFontPrint(twitterfont, 55, 133, TWITTER, "Twit it");
		triFontPrint(twitterfont, 55, 163, TWITTER, "Timeline");
		triFontPrint(twitterfont, 55, 193, TWITTER, "Settings");
		triFontPrint(twitterfont, 55, 223, TWITTER, "Exit");

		triImageNoTint();
		triSwapbuffers();
		//sceKernelDelayThread(200);
	}

	fadeOut();
	unloadAll();
	sceKernelExitGame();
	return 0;
}

