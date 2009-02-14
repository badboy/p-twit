#include <pspkernel.h>
#include <arpa/inet.h>
#include <string.h>
#include <pspctrl.h>

extern "C" {
#include "../tri/triTypes.h"
#include "../tri/triLog.h"
#include "../tri/triMemory.h"
#include "../tri/triInput.h"
#include "../tri/triNet.h"
}

#include "../base64.h"
#include "netextended.h"

/*
	sprintf(buffer, "%s:%s", request->username, request->password);
	char *base64_encoded = base64_encode(buffer, strlen(buffer));
*/


PSP_MODULE_INFO("twitternettest", 0x0, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(20480);

static int isrunning = 1;

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

int main(int argc, char **argv)
{
	SetupCallbacks();
	
	triLogInit();

	triNetInit();

	triMemoryInit();

	pspDebugScreenInit();

	while(!triNetSwitchStatus())
	{
		pspDebugScreenSetXY(0, 0);
		pspDebugScreenPrintf("Turn the wifi switch on!\n");
	}

	triNetConfig netConfigs[6];

	triSInt numConfigs = triNetGetConfigs(netConfigs, 6);
	
	if( numConfigs == 0 )
	{
	      pspDebugScreenPrintf("No connections found.\n");
	      return 0;
	}

	int i;
	
	for ( i = 0; i < numConfigs; i++ )
	{
		     pspDebugScreenPrintf("Index: %i, Name: %s\n", netConfigs[i].index, netConfigs[i].name.asString);
	}
	
	pspDebugScreenPrintf("Connecting to first config\n");
	triNetConnect(&netConfigs[0]);

	pspDebugScreenPrintf("Connected!\n");

//	const char *ts = "badboy_:twitterrulez";
	const char *ts = "wiki:pedia_";
	std::string encoded = base64_encode((unsigned char const*)ts, strlen(ts));
//	HTTPRequest req = { "twitter.com", "/statuses/friends_timeline.xml", 0, " ", "twitternet", encoded};
	HTTPRequest req = { "192.168.1.37", "/friends_timeline.xml", 0, " ", "twitternet", encoded};

	pspDebugScreenPrintf("bytes read: %d\n", NetGetUrlExtended(&req));
	triNetDisconnect();
	pspDebugScreenPrintf("%s\n\noutsize: %d\n", req.responseHeader.c_str(), req.body.size());

	FILE *fp = fopen("out.txt", "w");
	fwrite(req.body.c_str(), 1, req.body.size(), fp);
	fclose(fp);

	while(isrunning);

	triMemoryCheck();
	triMemoryShutdown();
	sceKernelExitGame();
	return 0;
}


/**
* User-Agent: Mozilla/5.0 (X11; U; Linux i686; de; rv:1.9b5) Gecko/2008050509 Firefox/3.0b5
*/
