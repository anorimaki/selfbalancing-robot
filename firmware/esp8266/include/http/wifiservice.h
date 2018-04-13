#ifndef HTTP_WIFI_SERVICE_H
#define HTTP_WIFI_SERVICE_H

#include <ESPAsyncWebServer.h>
#include "io/display.h"

namespace http {

class WifiService {
public:
	WifiService( AsyncWebServer* server, const String& path, io::Display* display );

private:
	void handleGetSoftApConfig( AsyncWebServerRequest *request );
	void handleSetSoftApEnabled( AsyncWebServerRequest *request, char* data, size_t len );
	void handleSetSoftApBss( AsyncWebServerRequest *request, char* data, size_t len );

	void handleGetStationConfig( AsyncWebServerRequest *request );
	void handleSetStationEnabled( AsyncWebServerRequest *request, char* data, size_t len );
	void handleSetStationBss( AsyncWebServerRequest *request, char* data, size_t len );
	void handleSetStationDhcp( AsyncWebServerRequest *request, char* data, size_t len );
	void handleSetStationNetwork( AsyncWebServerRequest *request, char* data, size_t len );
	void handleGetSsids( AsyncWebServerRequest *request );
};

}

#endif
