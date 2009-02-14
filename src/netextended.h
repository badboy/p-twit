#ifndef NETEXTENDED_H_
#define NETEXTENDED_H_

#include <string>

typedef struct _HTTPRequest
{
	std::string host;
	std::string page;
	int method; // 1 == GET, 2 == POST, 0 == No Data
	std::string data;
	std::string useragent;
	std::string auth;
	int responseCode;
	std::string responseText;
	std::string responseHeader;
	std::string body;
} HTTPRequest;

triSInt NetGetUrlExtended(HTTPRequest *request);

#endif
