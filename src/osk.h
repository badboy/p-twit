#ifndef OSK_H_
#define OSK_H_

extern "C" {
#include "tri/triTypes.h"
#include "tri/triGraphics.h"
}

char *SonyOSK(int len, const char *_desc, const char *inittext, triImage* img);

#endif
