#include "http/httpserver.h"
#include "util/trace.h"
#include "util/printbuffer.h"
#include "ArduinoJson.h"
#include "StreamString.h"
#include "util/arduino_stl_support.h"
#include <vector>
#include <algorithm>
#include <stdio.h>


namespace http
{


void Server::init( motion::Motors* motors )
{
	m_motors = motors;

	m_impl.on( "/", std::bind( &Server::handleRoot, this ) );
	m_impl.on( "/motors/pitch/state", std::bind( &Server::handleMotorsPitch, this ) );
	m_impl.begin();
	m_impl.client().setNoDelay(1);		//Just for performance
}


void Server::handleMotorsPitch() {
	std::vector<motion::Motors::PitchState> pitches;
	if ( !m_motors->pitchState(pitches) ) {
		sendError( "Error reading pitch states" );
		return;
	}

	DynamicJsonBuffer jsonBuffer;
	JsonArray& array = jsonBuffer.createArray();
	std::for_each( pitches.begin(), pitches.end(), [&array](const motion::Motors::PitchState& pitch) {
			JsonObject& entry = array.createNestedObject();
			entry["i"] = pitch.index;
			entry["p_err"] = pitch.state.previous_error;
			entry["i_err"] = pitch.state.integral_error;
			entry["tar"] = pitch.state.target;
			entry["cur"] = pitch.state.current;
		} );

					//Send headers
	m_impl.setContentLength( array.measureLength() );
	m_impl.send( 200, "application/json" );

					//Send body
	WiFiClient client = m_impl.client();

	io::PrintBuffer<1760> printBuf(&client);
	array.printTo( printBuf );
	printBuf.flush();
}


void Server::sendError( const char* message ) {
	String content = String("<html>\
			  <head>\
			    <title>ESP8266 error</title>\
			    <style>\
			      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
			    </style>\
			  </head>\
			  <body>\
			    <h1>") + message + "</h1>\
			  </body>\
			</html>";
	m_impl.send( 500, "text/html", content );
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
						<title>ESP8266 Self balancing robot</title>\
						<style>\
						  body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
						</style>\
					  </head>\
					  <body>\
						<h1>Hello from ESP8266!</h1>\
						<p>Uptime: %02d:%02d:%02d</p>\
					  </body>\
					</html>",
			hr, min % 60, sec % 60 );
	m_impl.send ( 200, "text/html", temp );
}



}



