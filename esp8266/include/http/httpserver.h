#ifndef INCLUDE_HTTP_HTTPSERVER_H_
#define INCLUDE_HTTP_HTTPSERVER_H_

#include <ESP8266WebServer.h>
#include "motion/motors.h"

namespace http
{

class Server
{
public:
	void init( motion::Motors* motors );

	ESP8266WebServer& impl() {
		return m_impl;
	}


private:
	void handleMotorsPitch();
	void handleRoot();

private:
	ESP8266WebServer m_impl;
	motion::Motors* m_motors;
};

}



#endif /* INCLUDE_HTTP_HTTPSERVER_H_ */
