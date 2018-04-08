#ifndef HTTP_BASIC_RESPONSES_H
#define HTTP_BASIC_RESPONSES_H

#include <ESPAsyncWebServer.h>

namespace http
{

static const char* CONTENT_TYPE_JSON = "application/json";

template <typename T>
static void sendBasicJson( AsyncWebServerRequest* request, T& content )
{
	AsyncResponseStream *response = request->beginResponseStream(CONTENT_TYPE_JSON);
	content.printTo(*response);
	request->send(response);
}

static void sendNoContent( AsyncWebServerRequest* request )
{
	request->send(204);
}

void sendError( AsyncWebServerRequest* request );

}

#endif
