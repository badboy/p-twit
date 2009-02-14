extern "C" {
#include "tri/triTypes.h"
#include "tri/triLog.h"
#include "tri/triNet.h"
}

#include "helpers.h"
#include "netextended.h"

triSInt NetGetUrlExtended(HTTPRequest *request)
{
	triLogPrint("triNet: triNetGetUrlExtended(): start\n");

	std::string header;
	triLogPrint("host: %s\n", request->host.c_str());

	if(request->method == 1 && request->data.size() > 0) // GET + args
		header = "GET "+ request->page +"?"+ request->data +" HTTP/1.1\r\n";
	else if(request->method == 2) // POST
		header = "POST "+ request->page +" HTTP/1.1\r\n";
	else
		header = "GET "+ request->page +" HTTP/1.1\r\n";

	header += "Host: "+ request->host +"\r\n";

	if(request->useragent.size() > 0)
		header += "User-Agent: "+ request->useragent +"\r\n";
	else
		header += "User-Agent: PSP (PlayStation Portable); 2.00 <Homebrew>\r\n";

	header += "Accept-Charset: UTF-8,*\r\n";

	if(request->auth.size() > 0)
	{
		header += "Authorization: Basic "+ request->auth +"\r\n";
	}

	int data_size = request->data.size();
	if(request->method == 2 && data_size > 0)
	{
		header += "Content-Type: application/x-www-form-urlencoded\r\n";
		char sbuf[128];
		sprintf(sbuf, "%d", data_size);
		header += "Content-Length: "+ std::string(sbuf) +"\r\n";
	}
	header += "\r\n";
	int header_size = header.size();

	triLogPrint("triNet: triNetGetUrlExtended(): header:\n%s\n\n", header.c_str());

	triSocket mySocket = triNetSocketCreate();
        triChar *theHost = triNetResolveHost((triChar*)request->host.c_str());
        triNetSocketConnect(mySocket, theHost, 80);

        triNetSocketSend(mySocket, header.c_str(), header_size);
	if(request->data.size() > 0)
	        triNetSocketSend(mySocket, request->data.c_str(), data_size);

        signed int bytesReceived, allBytes = 0;
	size_t found;

        triSInt headerFinished = 0;

	triChar buffer[1023];
	memset(buffer, 0, 1023);
	std::string buf, response;

	header.clear();
	triLogPrint("data send...receiving...\n");

	while(1)
	{
		bytesReceived = recv(mySocket, buffer, 1023, 0);
		triLogPrint("bytesReceived: %d\n", bytesReceived);
		if(bytesReceived <= 0) break;
		buffer[bytesReceived] = 0;
		allBytes += bytesReceived;

		buf = buffer;
		if(buf.size() > 0)
		{
			if(headerFinished)
			{
				response += buf;
			}
			else
			{
				found = buf.find("\r\n\r\n");
				if(found == std::string::npos)
					header += buf;
				else
				{
					header += buf.substr(0, found);
					response += buf.substr(found+4);
					headerFinished = 1;
				}
			}
		}
	}

	request->responseCode = 0;
	request->responseText = "";
	found = header.find("HTTP/1.1 ");
	if(found != std::string::npos)
	{
		buf = header.substr(found+9, found+found+11);
		request->responseCode = atoi(buf.c_str());
		size_t found2;
		found2 = header.find("\r\n");
		if(found != std::string::npos)
		{
			request->responseText = header.substr(found+9, found2-9);
		}
	}
	str_gsub("\r", "", header);
	str_gsub("\r", "", response);
	request->responseHeader = header;
	request->body = response;
	return allBytes;
}
