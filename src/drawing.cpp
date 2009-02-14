#include "includes.h"

void drawMessage(const char* message)
{
	triDrawImage2( 260.f, 65.f, arrow );
	triDrawRect( 30.f, 76.f, 340.f, 23.f, 0xFFFFFFFF );

	triFontActivate(verdana);
	triFontPrint(verdana, 35, 80, 0xFF000000, message);
}
