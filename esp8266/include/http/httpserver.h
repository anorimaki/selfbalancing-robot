#ifndef INCLUDE_HTTP_HTTPSERVER_H_
#define INCLUDE_HTTP_HTTPSERVER_H_

#include <ESP8266WebServer.h>
#include "motion/motors.h"

namespace http
{

class Server
{
public:
	void init( motion::Motors* motors, io::Display* display );

	ESP8266WebServer& impl() {
		return m_impl;
	}

private:
	void handleMotorsPitch();
	void handleMotorsPIDSettingsPitch();
	void handleMotorsSetPIDSettingsPitch();
	void handleRoot();
	void handleOptionsRequest();
	void handleNotFound();
	void sendError( const char* message );

	void handleRequest( void (Server::*handler)() );

private:
	ESP8266WebServer m_impl;
	motion::Motors* m_motors;
	io::Display* m_display;
};

}



#endif /* INCLUDE_HTTP_HTTPSERVER_H_ */
