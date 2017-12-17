#include "http/httpserver.h"
#include "util/trace.h"
#include "util/printbuffer.h"
#include "ArduinoJson.h"
#include "StreamString.h"
#include "util/arduino_stl_support.h"
#include "motors_i2c_model.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <stdio.h>



int snprintf( char * s, size_t n, const char * format, ... );



namespace http
{

#define ACCESS_CONTROL_REQUEST_HEADERS "Access-Control-Request-Headers"
#define ACCESS_CONTROL_REQUEST_METHOD "Access-Control-Request-Method"
#define ACCESS_CONTROL_ALLOW_HEADERS "Access-Control-Allow-Headers"
#define ACCESS_CONTROL_ALLOW_METHODS "Access-Control-Allow-Methods"

static const char* headersToCollect[] = {
		ACCESS_CONTROL_REQUEST_HEADERS, ACCESS_CONTROL_REQUEST_METHOD
};


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


static void sendError( ESP8266WebServer& impl, const char* message ) {
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
	impl.send( 500, "text/html", content );
}

typedef std::function<void()> HandlerFunction;

static void handleRequest( io::Display* display, HandlerFunction handler )
{
	display->httpRequestBegin();
	handler();
	display->httpRequestEnd();
}


/************************************************************************/
// PidService
/************************************************************************/
PidService::PidService( ESP8266WebServer* impl, const std::string& path,
						motion::PidEngine* pidEngine, io::Display* display ):
						m_impl(impl), m_pidEngine(pidEngine), m_display( display ) {
	std::string statePath = path + "/state";
	std::string settingsPath = path + "/settings";

	HandlerFunction stateFn = std::bind( &PidService::handleState, this );
	m_impl->on( statePath.c_str(), HTTPMethod::HTTP_GET, std::bind( &handleRequest, m_display, stateFn ) );

	HandlerFunction settingsFn = std::bind( &PidService::handleSettings, this );
	m_impl->on( settingsPath.c_str(), HTTPMethod::HTTP_GET, std::bind( &handleRequest, m_display, settingsFn ) );

	HandlerFunction setSettingsFn = std::bind( &PidService::handleSetSettings, this );
	m_impl->on( settingsPath.c_str(), HTTPMethod::HTTP_PUT, std::bind( &handleRequest, m_display, setSettingsFn ) );
}


static StaticJsonBuffer<1024*20> jsonBuffer;


void PidService::handleState() {
	std::vector<::PIDStateEntry> states;
	if ( !m_pidEngine->state(states) ) {
		sendError( *m_impl, "Error reading PID states" );
		return;
	}

	jsonBuffer.clear();
	JsonArray& array = jsonBuffer.createArray();
	std::for_each( states.begin(), states.end(), [&array](const ::PIDStateEntry& state) {
			JsonObject& entry = array.createNestedObject();
			entry["i"] = state.index;
			entry["tar"] = state.state.target;
			entry["cur"] = state.state.current;
			entry["p_err"] = state.state.previous_error;
			entry["i_err"] = state.state.integral_error;
		} );

	sendJson( *m_impl, array );
}


void PidService::handleSettings() {
	::PIDSettings settings;
	if ( !m_pidEngine->settins(settings) ) {
		sendError( *m_impl, "Error reading PID settings" );
		return;
	}

	jsonBuffer.clear();
	JsonObject& jsonSetings = jsonBuffer.createObject();
	jsonSetings["integral"] = settings.k_i;
	jsonSetings["proportional"] = settings.k_p;
	jsonSetings["derivative"] = settings.k_d;

	sendJson( *m_impl, jsonSetings );
}


void PidService::handleSetSettings() {
	jsonBuffer.clear();
	String body = m_impl->arg("plain");
	const JsonObject& root = jsonBuffer.parseObject( body );

	motion::Motors::PIDSettings settings;
	settings.k_i = root["integral"];
	settings.k_p = root["proportional"];
	settings.k_d = root["derivative"];
	if ( !m_pidEngine->setSettins(settings) ) {
		sendError( *m_impl, "Error settings pitch PID settings" );
		return;
	}

	//Send No content
	m_impl->setContentLength( 0 );
	m_impl->send( 204, NULL );
}



/************************************************************************/
// Server
/************************************************************************/
Server::Server( motion::Motors* motors, io::Display* display )
{
	m_motors = motors;
	m_display = display;

	m_impl.on( "/", std::bind( &Server::handleRoot, this ) );

	m_pitchService = new PidService( &m_impl, "/pitch", &m_motors->pitch(), m_display );
	m_speedService = new PidService( &m_impl, "/speed", &m_motors->speed(), m_display );

	m_impl.onNotFound([this](){
			if ( m_impl.method() == HTTP_OPTIONS ) {
				handleOptionsRequest();
				return;
			}
			handleRequest( m_display, std::bind( &Server::handleNotFound, this ) );
		});

	m_impl.collectHeaders( headersToCollect, sizeof(headersToCollect)/sizeof(const char*) );
	m_impl.begin();
	m_impl.client().setNoDelay(1);		//Just for performance
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



