#ifndef HELPERS_H_
#define HELPERS_H_

#include <string>
#include <stdarg.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <psputils.h>
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"

#include "includes.h"
#include "init.h"



std::string split(std::string in, unsigned int after=41);
int str_gsub(std::string pattern, std::string replacement, std::string& input);
int str_sub(std::string pattern, std::string replacement, std::string& input);
const char* textValue(TiXmlElement* e);
void str_tolower(std::string &str);
void ErrorExit(int milisecs, char *fmt, ...);
void GraphicErrorExit(int milisecs, const char *msg);

#endif
