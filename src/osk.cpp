#include "osk.h"
#include <pspkernel.h>
#include <psptypes.h>
#include <psputility.h>
extern "C" {
#include "tri/triLog.h"
#include "tri/triTypes.h"
#include "tri/triGraphics.h"
}

#include <cstdio>
#include <string.h>


#define MAX_TEXT_LENGTH (255)

char *SonyOSK(int len, const char *_desc, const char *inittext, triImage* img)
{
	unsigned short intext[MAX_TEXT_LENGTH];
	unsigned short outtext[MAX_TEXT_LENGTH];
	unsigned short desc[MAX_TEXT_LENGTH];
	
	memset(intext, 0, MAX_TEXT_LENGTH * sizeof(unsigned short));
	memset(outtext, 0, MAX_TEXT_LENGTH * sizeof(unsigned short));
	memset(desc, 0, MAX_TEXT_LENGTH * sizeof(unsigned short));
	
	int i;
	int txt_len = strlen(_desc);
	for(i=0;i<txt_len;i++)
	{
		desc[i] = _desc[i];
	}
	desc[txt_len] = 0;

	txt_len = strlen(inittext);
	for(i=0;i<txt_len;i++)
	{
		intext[i] = inittext[i];
	}
	intext[txt_len] = 0;	
	
	SceUtilityOskData data;
	
	memset(&data, 0, sizeof(SceUtilityOskData));
	data.language = PSP_UTILITY_OSK_LANGUAGE_DEFAULT; // Use system default for text input
	data.lines = 1;
	data.unk_24 = 1;
	data.inputtype = PSP_UTILITY_OSK_INPUTTYPE_ALL; // Allow all input types
	data.desc = desc;
	data.intext = intext;
	data.outtextlength = MAX_TEXT_LENGTH;
	data.outtextlimit = len;
	data.outtext = outtext;
	
	SceUtilityOskParams params;
	memset(&params, 0, sizeof(params));
	params.base.size = sizeof(params);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &params.base.language);
	//sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &params.base.buttonSwap);
	params.base.buttonSwap = 1;
	params.base.graphicsThread = 17;
	params.base.accessThread = 19;
	params.base.fontThread = 18;
	params.base.soundThread = 16;
	params.datacount = 1;
	params.data = &data;

	sceUtilityOskInitStart(&params);

	int done = 0;
	while(!done)
	{
		triClear( 0 );
		if(img)
			triDrawImage2( 0.f, 0.f, img );

		sceGuFinish();
		sceGuSync(0,0);
		int stat = sceUtilityOskGetStatus();
		switch(stat)
		{
			case PSP_UTILITY_DIALOG_INIT:
				break;
			
			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilityOskUpdate(1);
				break;
			
			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilityOskShutdownStart();
				break;
			
			case PSP_UTILITY_DIALOG_FINISHED:
				break;
				
			case PSP_UTILITY_DIALOG_NONE:
				done = 1;
				
			default :
				break;
		}

		triSwapbuffers();
	}

	int j;
	char buf[sizeof(unsigned)];
	char *ret = new char[len+1];
	memset(ret, 0, len+1);
	for(j = 0; data.outtext[j]; j++)
	{
		unsigned c = data.outtext[j];
		
		if(32 <= c && c <= 127) // print ascii only
		{
			sprintf(buf, "%c", data.outtext[j]);
			strcat(ret, buf);
		}
	}
	ret[j] = 0;
	triLogPrint("ret: %s (%d)\n", ret, strlen(ret));
	return ret;
}
