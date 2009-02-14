#include <pspdebug.h>
#include "helpers.h"
#include "init.h"
#include "datehelpers.h"

extern "C" {
#include "tri/triLog.h"
}

triFont *verdana;
triFont *twitterfont;
triFont *verdanaSmall;
triFont *verdanaSmaller;

triImage *background;
triImage *twitterlogo;
triImage *bird;
triImage *arrow;

triImage *message_box;
triImage *message_box_layer;
triImage *reply_cross;
triImage *scroll;
triImage *scroller;

void init(int log)
{
	SetupCallbacks();
	pspDebugScreenInit();

	if(log)	triLogInit();

	int systemparam_tmp;
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_TIMEZONE, &systemparam_tmp);
	triLogPrint("timezone: %d\n", systemparam_tmp);
	timediff = systemparam_tmp*60;
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_DAYLIGHTSAVINGS, &systemparam_tmp);
	triLogPrint("daylightsaving: %d\n", systemparam_tmp);
	if(systemparam_tmp) timediff += 3600;

	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_DATE_FORMAT, &systemparam_tmp);
	int dateformat = systemparam_tmp;
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_TIME_FORMAT, &systemparam_tmp);
	int timeformat = systemparam_tmp;
	set_clockformat(dateformat, timeformat);

	triMemoryInit();

	triInit( GU_PSM_8888 );
	triEnable(TRI_VBLANK);

	triInputInit();
	triFontInit();
/*
	triClear( TWITTER );
	triSwapbuffers();
*/

	loadMediaFiles();
	loadTimelineFiles();

	fadeIn();
}

void fadeIn()
{
	triClear( TWITTER );
	triSwapbuffers();

	triLogPrint("fadeIn(): start.\n");

	triU32 white[] = { 0xFFFFFFFF, 0xEEFFFFFF, 0xDDFFFFFF, 0xCCFFFFFF, 0xBBFFFFFF, 0xAAFFFFFF, 0x99FFFFFF, 0x88FFFFFF, 0x77FFFFFF, 0x66FFFFFF, 0x55FFFFFF, 0x44FFFFFF, 0x33FFFFFF, 0x22FFFFFF, 0x11FFFFFF, 0x00FFFFFF };

	int run = 0;
	for(run=0; run < 17; run++)
	{
		triClear( TWITTER );

		triDrawImage2( 0.f, 0.f, background );
		triDrawImage2( 135.f, 111.f, twitterlogo );
		if(run < 16)
			triDrawRect( 0.f, 0.f, 480.f, 272.f, white[run] );

		triSwapbuffers();
		sceKernelDelayThread(50000);
	}

	triClear( 0 );
	triDrawImage2( 0.f, 0.f, background );
	triSwapbuffers();

	triLogPrint("fadeIn(): done.\n");
}

void fadeOut()
{
	triClear( TWITTER );
	triSwapbuffers();

	triLogPrint("fadeOut(): start.\n");

	triU32 white[] = { 0xFFFFFFFF, 0xEEFFFFFF, 0xDDFFFFFF, 0xCCFFFFFF, 0xBBFFFFFF, 0xAAFFFFFF, 0x99FFFFFF, 0x88FFFFFF, 0x77FFFFFF, 0x66FFFFFF, 0x55FFFFFF, 0x44FFFFFF, 0x33FFFFFF, 0x22FFFFFF, 0x11FFFFFF, 0x00FFFFFF };

	int run = 0;
	for(run=16; run >= 0; run--)
	{
		triClear( 0xFFFFFFFF );

		triDrawImage2( 0.f, 0.f, background );
		triDrawImage2( 135.f, 111.f, twitterlogo );
		triDrawRect( 0.f, 0.f, 480.f, 272.f, white[run] );

		triFontActivate(verdana);
		triFontPrint(verdana, 10, 220, TWITTER, "by BadBoy_");
		triFontPrint(verdana, 10, 240, TWITTER, "visit http://badboy.pytalhost.de/p-twit");
		triImageNoTint();
		triSwapbuffers();
		sceKernelDelayThread(50000);
	}

	triLogPrint("fadeOut(): done.\n");
}

void unloadAll()
{
	triFontUnload( twitterfont );
	triFontUnload( verdana );
	triFontShutdown();

	triImageFree( background );
	triImageFree( twitterlogo );
	triImageFree( bird );
	triImageFree( arrow );

	triImageFree( message_box );
	triImageFree( message_box_layer );
	triImageFree( reply_cross );
	triImageFree( scroll );
	triImageFree( scroller );

	triNetDisconnect();

	triInputShutdown();

	triClose();
	triMemoryCheck();
	triMemoryShutdown();
}

void loadMediaFiles()
{
	twitterfont = triFontLoad("font/twitter.ttf", 30, TRI_FONT_SIZE_POINTS, 384, TRI_FONT_VRAM);
	if(!twitterfont)
	{
		ErrorExit(5000, "Error loading font 'twitterfont'!\n");
	}
	verdana = triFontLoad("font/verabd.ttf", 14, TRI_FONT_SIZE_POINTS, 384, TRI_FONT_VRAM);
	if(!verdana)
	{
		ErrorExit(5000, "Error loading font 'verdana'!\n");
	}

	background = triImageLoad("img/background.tga", 0);
	if (background==0)
	{
		ErrorExit(5000, "Error loading background.tga!\n");
	}

	triClear( TWITTER );
	triDrawImage2( 0.f, 0.f, background );
	triFontActivate(twitterfont);
	triFontPrint(twitterfont, 150, 130, TWITTER, "Loading...");
	triImageNoTint();
	triSwapbuffers();


	verdanaSmall = triFontLoad("font/verdana.ttf", 12, TRI_FONT_SIZE_POINTS, 384, TRI_FONT_VRAM);
	verdanaSmaller = triFontLoad("font/verdana.ttf", 10, TRI_FONT_SIZE_POINTS, 384, TRI_FONT_VRAM);

	if(!verdanaSmall)
	{
		GraphicErrorExit(5000, "Error loading font: verdanaSmall!");
	}

	if(!verdanaSmaller)
	{
		GraphicErrorExit(5000, "Error loading font: verdanaSmaller!");
	}

	twitterlogo = triImageLoad("img/twitter.tga", 0);
	if (twitterlogo==0)
	{
		ErrorExit(5000, "Error loading twitter.tga!\n");
	}

	bird = triImageLoad("img/bird.tga", 0);
	if (bird==0)
	{
		ErrorExit(5000, "Error loading bird.tga!\n");
	}

	arrow = triImageLoad("img/arrow.png", 0);
	if (arrow==0)
	{
		ErrorExit(5000, "Error loading arrow.png!\n");
	}
}

void loadTimelineFiles()
{
	message_box = triImageLoad("img/message_box.png", 0);
	if(message_box==0)
	{
		ErrorExit(5000, "Error loading message_box.png!\n!");
	}

	message_box_layer = triImageLoad("img/message_box_layer.png", 0);
	if(message_box_layer==0)
	{
		ErrorExit(5000, "Error loading message_box_layer.png!\n!");
	}

	reply_cross = triImageLoad("img/reply_cross.png", 0);
	if(reply_cross==0)
	{
		ErrorExit(5000, "Error loading reply_cross.png!\n!");
	}

	scroll = triImageLoad("img/scroll.png", 0);
	if(scroll==0)
	{
		ErrorExit(5000, "Error loading scroll.png!\n!");
	}

	scroller = triImageLoad("img/scroller.png", 0);
	if(scroller==0)
	{
		ErrorExit(5000, "Error loading scroller.png!\n!");
	}

}

int triNetInited = 0;
int initNet(int connection)
{
	if(!triNetInited)
	{
		if(!triNetInit())
		{
			ErrorExit(5000, "Can't initialize net. Exiting.\n");
			return 0;
		}
		else triNetInited = 1;
	}
	triNetConfig netConfigs[10];

	triSInt numConfigs = triNetGetConfigs(netConfigs, 10);
	
	if( numConfigs == 0 )
	{
		triLogError("initNet: No connections found.\n");
		return 0;
	}

	if( connection > numConfigs ) 
	{
		triLogError("initNet: Choosed connection is not available.\n");
		return 0;
	}

	triLogPrint("initNet: Connecting to config #%d\n", connection);
	if(!triNetConnect(&netConfigs[connection-1]))
	{
		triLogError("initNet: error while connecting.\n");
		return 0;
	}
	triLogError("initNet: Connected\n");
	return 1;
}

int isrunning = 1;

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	isrunning = 0;
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread,
				     0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}
