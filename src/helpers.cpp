#include <string.h>
#include <algorithm>

#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"
#include "helpers.h"

extern "C" {
#include "tri/triLog.h"
}

std::string split(std::string in, unsigned int after)
{
	if(after <= 0) return in;
	std::string out;
	if(in.find(" ") == std::string::npos)
	{
		while(in.size() > after)
		{
			out += in.substr(0, after);
			out += "\n";
			in = in.substr(after);
		}
		out += in;
		return out;
	}


	while(in.size() > after)
	{
		if(in[after] == ' ')
		{
			out += in.substr(0, after);
			in = in.substr(after);
		}
		else
		{
			int i = after;
			for(i=after;in[i] != ' ' && i > -1;i--);
			if(i <= 0) 
			{
				out += in.substr(0, after);
				in = in.substr(after);
			}
			else
			{
				out += in.substr(0, i);
				in = in.substr(i+1);
			}
		}
		out += "\n";
	}
	out += in;
	return out;
}
int str_gsub(std::string pattern, std::string replacement, std::string& input)
{
	size_t found = input.find(pattern);
	while(found > 0)
	{
		size_t size = input.size();
		if(found < size)
		{
			input.replace(found, pattern.size(), replacement);
		}
		else break;
		found = input.find(pattern);
	}
	return 1;
}

int str_sub(std::string pattern, std::string replacement, std::string& input)
{
	size_t found = input.find(pattern);
	size_t size = input.size();
	if(found < size)
	{
		input.replace(found, pattern.size(), replacement);
		return 1;
	}
	else return 0;
}

const char* textValue(TiXmlElement* e)
{
    TiXmlNode* first = e->FirstChild( );
    if ( first != 0 &&
         first == e->LastChild( ) &&
         first->Type( ) == TiXmlNode::TEXT )
    {
        // Das Element e hat ein einzelnes Kind vom Typ TEXT;
        // Inhalt zurückgeben.
        return first->Value( );
    } else {
            return ""; //throw runtime_error(string("falsches Element ") + e->Value( ));
    }
}

void str_tolower(std::string &str)
{
  std::transform(str.begin(), 
                 str.end(), 
                 str.begin(),
                 (int(*)(int)) std::tolower);
}


void ErrorExit(int milisecs, char *fmt, ...)
{
	va_list list;
	char msg[256];	

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	pspDebugScreenPrintf(msg);
	triLogError(msg);

	sceKernelDelayThread(milisecs*1000);
	sceKernelExitGame();
}

void GraphicErrorExit(int milisecs, const char *msg)
{
	triClear( TWITTER );
	triDrawImage2( 0.f, 0.f, background );
	triFontActivate(verdana);
	triFontPrint(verdana, 20, 130, WHITE, msg);
	triImageNoTint();
	triSwapbuffers();

	triLogError("%s\n", msg);

	sceKernelDelayThread(milisecs*1000);
	sceKernelExitGame();
}
