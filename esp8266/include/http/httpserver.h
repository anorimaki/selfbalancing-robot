#ifndef INCLUDE_HTTP_HTTPSERVER_H_
#define INCLUDE_HTTP_HTTPSERVER_H_

#include <ESP8266WebServer.h>

namespace http
{

class Server
{
public:
	void init();

	ESP8266WebServer& impl() {
		return m_impl;
	}


private:
	void handleRoot();

private:
	ESP8266WebServer m_impl;
};

}



#endif /* INCLUDE_HTTP_HTTPSERVER_H_ */
