#include "includes.h"
#include "options_menu.h"
#include "init.h"
#include "osk.h"
#include "CConfig.h"

int optionsMenu(Config *config)
{
	triFloat line_lenght[] = { 106.f, 106.f, 170.f, /*167.f, 183.f,*/ 159.f, 198.f };
	triFloat line_y[] = { 98.f, 118.f, 143.f, /*163.f, 183.f,*/ 223.f, 238.f };
	int max_menu=5, menu_pos=0;
	triLogPrint("entering Options menu.\n");
	int option_isrunning = 1, save = 0;

	triFloat text_positions[5][2] = { {110, 85}, {110, 105}, {174, 130}, {171,150}, {187,170} };

	triNetConfig netConfigs[10];
	triSInt numConfigs = triNetGetConfigs(netConfigs, 10);

	while (option_isrunning)
	{
		triInputUpdate();

		if(triInputPressed(PSP_CTRL_UP))
		{
			if(menu_pos > 0) --menu_pos;
			triLogPrint("option menupos: %d\n", menu_pos);
		}
		if(triInputPressed(PSP_CTRL_DOWN)) 
		{
			if(menu_pos < max_menu) ++menu_pos;
			triLogPrint("option menupos: %d\n", menu_pos);
		}

		if(triInputPressed(PSP_CTRL_RIGHT) && menu_pos == 2) 
		{
			if(config->wlan_connection < numConfigs)
				++config->wlan_connection;
			else
				config->wlan_connection = 1;
		}

		if(triInputPressed(PSP_CTRL_LEFT) && menu_pos == 2)
		{
			if(config->wlan_connection > 1)
				--config->wlan_connection;
			else
				config->wlan_connection = numConfigs;
		}

		if(triInputPressed(PSP_CTRL_CROSS))
		{
			switch(menu_pos)
			{
				case 0: 
				{
					char *buf = SonyOSK(255, "Your Nickname", config->user_nick.c_str(), background);
					if(buf && strlen(buf) > 0)
						config->user_nick = buf;
					break;
				}
				case 1:
				{
					char *buf = SonyOSK(255, "Your Password", config->user_password.c_str(), background);
					if(buf && strlen(buf) > 0)
						config->user_password = buf;
					break;
				}
				case 3: save = 1; option_isrunning = 0; continue; break;
				case 4: save = 0; option_isrunning = 0; continue; break;
				default: break;
			}
		}

		if(triInputPressed(PSP_CTRL_LTRIGGER))
		{
			triImage* snap = triImageGet( 0, 0, 480, 272 );
			triImageSavePng( "options_screenshot.png", snap, 0 ); 
			triImageFree( snap );
			triLogPrint("Screenshot saved to options_screenshot.png\n");
		}

		if(triInputPressed(PSP_CTRL_TRIANGLE))
		{
			triLogPrint("leave option menu\n");
			option_isrunning = 0;
			continue;
		}

		triClear(0);

		triDrawImage2( 0.f, 0.f, background );
		triDrawImage2( 250.f, 10.f, twitterlogo );

		triDrawImage2( 30.f, 10.f, arrow );
		triDrawRect( 10.f, 21.f, 200.f, 23.f, 0xFFFFFFFF );

		triDrawImage2( 30.f, 60.f, arrow );
		triDrawRect( 10.f, 71.f, 460.f, 180.f, 0xFFFFFFFF );

		triDrawLine( 20.f, line_y[menu_pos], line_lenght[menu_pos], line_y[menu_pos], TWITTER );

		triFontActivate(verdana);
		triFontPrint(verdana, 60, 25, BLACK, "Settings");

		triFontActivate(verdanaSmall);
		triFontPrint(verdanaSmall, 18, 85, BLACK, "Nickname: ");
		triFontPrint(verdanaSmall, 18, 105, BLACK, "Password: ");

		triFontPrint(verdanaSmall, 18, 130, BLACK, "WLAN Connection: ");

		triFontPrint(verdanaSmall, 18, 210, BLACK, "Save and Return");
		triFontPrint(verdanaSmall, 18, 225, BLACK, "Return without saving");

		/* print Config */

		triFontPrint(verdanaSmall, text_positions[0][0], text_positions[0][1], BLACK, config->user_nick.c_str());
		triFontPrint(verdanaSmall, text_positions[1][0], text_positions[1][1], BLACK, (config->user_password.size() > 0 ? "*********" : ""));
		triFontPrintf(verdanaSmall, text_positions[2][0], text_positions[2][1], BLACK, "%d (%s)", config->wlan_connection, netConfigs[config->wlan_connection-1].name);

		triImageNoTint();
		triSwapbuffers();
	}

	int saved = 0;
	if(save)
	{
		saved = config->save() ? 1 : 2;
		triLogPrint("saving: %s\n", saved == 1 ? "ok" : "error");
	}
	triLogPrint("leavin Options menu (save: %d).\n", save);
	triClear( 0 );
	triSwapbuffers();
	sceKernelDelayThread(2000);
	return saved;
}
