#ifndef INCLUDE_HTTP_HTTPSERVER_H_
#define INCLUDE_HTTP_HTTPSERVER_H_

#include <ESP8266WebServer.h>
#include "motion/motors.h"

namespace http
{


class PidService
{
public:
	PidService( ESP8266WebServer* impl, const std::string& path,
				motion::PidEngine* pidEngine, io::Display* display );

	void handleState();
	void handleSettings();
	void handleSetSettings();

private:
	ESP8266WebServer* m_impl;
	motion::PidEngine* m_pidEngine;
	io::Display* m_display;
};


class Server
{
public:
	Server( motion::Motors* motors, io::Display* display );

	ESP8266WebServer& impl() {
		return m_impl;
	}

private:
	void handleRoot();
	void handleOptionsRequest();
	void handleNotFound();

private:
	ESP8266WebServer m_impl;
	motion::Motors* m_motors;
	io::Display* m_display;
	PidService* m_speedService;
	PidService* m_pitchService;
};

}



#endif /* INCLUDE_HTTP_HTTPSERVER_H_ */
