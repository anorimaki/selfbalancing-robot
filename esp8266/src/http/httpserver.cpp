#include "http/httpserver.h"
#include "ArduinoJson.h"



namespace http
{




void Server::init( motion::Motors* motors )
{
	m_motors = motors;

	m_impl.on( "/", std::bind( &Server::handleRoot, this ) );
	m_impl.on( "/motors/pitch/state", std::bind( &Server::handleMotorsPitch, this ) );
	m_impl.begin();
}


void Server::handleMotorsPitch() {
	motion::Motors::Pitch::State pitch;
	m_motors->pitchState(pitch);

	StaticJsonBuffer<1024> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root["previous_error"] = pitch.previous_error;
	root["integral_error"] = pitch.integral_error;
	root["target"] = pitch.target;
	root["current"] = pitch.current;

					//Send headers
	m_impl.setContentLength( root.measureLength() );
	m_impl.send( 200, "application/json" );

					//Send body
	WiFiClient client = m_impl.client();
	root.printTo( client );
}


void Server::handleRoot() {
	char temp[400];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf ( temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

		hr, min % 60, sec % 60
	);
	m_impl.send ( 200, "text/html", temp );
}



}



