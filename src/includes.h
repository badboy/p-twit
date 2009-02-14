#ifndef INCLUDES_H_
#define INCLUDES_H_

#include <pspkernel.h>
#include <pspdisplay.h>
#include <psputils.h>
#include <string>
#include <arpa/inet.h>

// triEngine 
extern "C"
{
#include "tri/triImage.h"
#include "tri/triGraphics.h"
#include "tri/triTypes.h"
#include "tri/triLog.h"
#include "tri/triMemory.h"
#include "tri/triFont.h"
#include "tri/triInput.h"
#include "tri/triNet.h"
}

// TinyXML
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"

#include "CConfig.h"

#endif
