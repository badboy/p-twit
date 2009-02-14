#ifndef INIT_H_
#define INIT_H_

#include "includes.h"

enum colors
{
	WHITE =	0xFFFFFFFF,
	BLACK = 0xFF000000,
	TWITTER = 0xFFffcc33
};

extern triFont *verdana;
extern triFont *twitterfont;
extern triFont *verdanaSmall;
extern triFont *verdanaSmaller;

extern triImage *background;
extern triImage *twitterlogo;
extern triImage *bird;
extern triImage *arrow;

extern triImage *message_box;
extern triImage *message_box_layer;
extern triImage *reply_cross;
extern triImage *scroll;
extern triImage *scroller;

extern int isrunning;

void init(int log);
void fadeIn();
void fadeOut();
void unloadAll();
void loadMediaFiles();
void loadTimelineFiles();

int initNet(int connection);

int exit_callback(int arg1, int arg2, void *common);
int CallbackThread(SceSize args, void *argp);
int SetupCallbacks(void);

#endif
