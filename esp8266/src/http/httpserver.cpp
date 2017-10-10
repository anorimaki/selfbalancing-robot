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

template <typename T>
static void sendJson( ESP8266WebServer& server, T& content )
{
			//Send headers
	server.setContentLength( content.measureLength() );
	server.send( 200, "application/json" );

			//Send body
	WiFiClient client = server.client();
	io::PrintBuffer<1760> printBuf(&client);
	content.printTo( printBuf );
	printBuf.flush();
}


#define ACCESS_CONTROL_REQUEST_HEADERS "Access-Control-Request-Headers"
#define ACCESS_CONTROL_REQUEST_METHOD "Access-Control-Request-Method"
#define ACCESS_CONTROL_ALLOW_HEADERS "Access-Control-Allow-Headers"
#define ACCESS_CONTROL_ALLOW_METHODS "Access-Control-Allow-Methods"


static const char* headersToCollect[] = {
		ACCESS_CONTROL_REQUEST_HEADERS, ACCESS_CONTROL_REQUEST_METHOD
};

void Server::init( motion::Motors* motors )
{
	m_motors = motors;

	m_impl.on( "/", std::bind( &Server::handleRoot, this ) );
	m_impl.on( "/motors/pitch/state", HTTPMethod::HTTP_GET, std::bind( &Server::handleMotorsPitch, this ) );
	m_impl.on( "/motors/pitch/pid", HTTPMethod::HTTP_GET, std::bind( &Server::handleMotorsPIDSettingsPitch, this ) );
	m_impl.on( "/motors/pitch/pid", HTTPMethod::HTTP_PUT, std::bind( &Server::handleMotorsSetPIDSettingsPitch, this ) );
	m_impl.onNotFound([this](){
		if ( m_impl.method() == HTTP_OPTIONS ) {
			handleOptionsRequest();
			return;
		}
		handleNotFound();
	});
	m_impl.collectHeaders( headersToCollect, sizeof(headersToCollect)/sizeof(const char*) );
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

	sendJson( m_impl, array );
}


void Server::handleMotorsPIDSettingsPitch() {
	motion::Motors::PIDSettings settings;
	if ( !m_motors->pitchPIDSettins(settings) ) {
		sendError( "Error reading pitch PID settings" );
		return;
	}

	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonSetings = jsonBuffer.createObject();
	jsonSetings["integral"] = settings.k_i;
	jsonSetings["proportional"] = settings.k_p;
	jsonSetings["derivative"] = settings.k_d;

	sendJson( m_impl, jsonSetings );
}


void Server::handleMotorsSetPIDSettingsPitch() {
	String body = m_impl.arg("plain");
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( body );

	motion::Motors::PIDSettings settings;
	settings.k_i = root["integral"];
	settings.k_p = root["proportional"];
	settings.k_d = root["derivative"];
	if ( !m_motors->setPitchPIDSettins(settings) ) {
		sendError( "Error settings pitch PID settings" );
		return;
	}

	//Send No content
	m_impl.setContentLength( 0 );
	m_impl.send( 204, NULL );
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


void Server::handleNotFound() {
	m_impl.send( 404, "text/html", "<h1>Resource not found</h1>" );
}


void Server::handleOptionsRequest() {
	String requestHeader = m_impl.header(ACCESS_CONTROL_REQUEST_HEADERS);

	if ( requestHeader.length() > 0 ) {
		m_impl.sendHeader( ACCESS_CONTROL_ALLOW_HEADERS, requestHeader );
	}

	requestHeader = m_impl.header(ACCESS_CONTROL_REQUEST_METHOD);
	m_impl.sendHeader( ACCESS_CONTROL_ALLOW_METHODS, (requestHeader.length()>0) ? requestHeader : "GET, POST, PUT, OPTIONS" );

	m_impl.send( 200, "text/plain", "" );
}


}



