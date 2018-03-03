#include "http/httpserver.h"
#include "util/trace.h"
#include "util/printbuffer.h"
#include "i2c/i2c.h"
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

#define ACCESS_CONTROL_ALLOW_ORIGIN "Access-Control-Allow-Origin"
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
	server.sendHeader( ACCESS_CONTROL_ALLOW_ORIGIN, "*" );
	server.setContentLength( content.measureLength() );
	server.send( 200, "application/json" );

			//Send body
	WiFiClient client = server.client();
	io::PrintBuffer<1760> printBuf(&client);
	content.printTo( printBuf );
	printBuf.flush();
}


static void sendNoContent( ESP8266WebServer& server )
{
	server.sendHeader( ACCESS_CONTROL_ALLOW_ORIGIN, "*" );
	server.setContentLength( 0 );
	server.send( 204, NULL );
}

static void sendError( ESP8266WebServer& impl ) {
	String content = String("<html>\
			  <head>\
			    <title>ESP8266 error</title>\
			    <style>\
			      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
			    </style>\
			  </head>\
			  <body>\
			    <h1>Error stack</h1>");

	while( !error::globalStack().empty() ) {
		content.concat( "<p>" );
		const error::StackItem& item = error::globalStack().top();
		content.concat( item.file() );
		content.concat( "," );
		content.concat( item.line() );
		content.concat( ": " );
		content.concat( item.message().c_str() );
		error::globalStack().pop();
	}

	content.concat("\
			  </body>\
			</html>");
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
		TRACE_ERROR( "Error reading PID states" );
		sendError(*m_impl);
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
		TRACE_ERROR( "Error reading PID settings" );
		sendError(*m_impl);
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
		TRACE_ERROR( "Error settings pitch PID settings" );
		sendError(*m_impl);
		return;
	}

	sendNoContent( *m_impl );
}



/************************************************************************/
// Server
/************************************************************************/
Server::Server( motion::Motors* motors, mpu::Mpu9250* mpu, io::Display* display ):
				m_motors(motors), m_display(display), m_mpu(mpu)
{
	m_impl.on( "/", std::bind( &Server::handleRoot, this ) );

	m_pitchService = new PidService( &m_impl, "/pitch", &m_motors->pitch(), m_display );
	m_speedService = new PidService( &m_impl, "/speed", &m_motors->speed(), m_display );
	m_headingService = new PidService( &m_impl, "/heading", &m_motors->heading(), m_display );

	HandlerFunction putTargets = std::bind( &Server::handlePutTargets, this );
	m_impl.on( "/targets", HTTPMethod::HTTP_PUT, std::bind( &handleRequest, m_display, putTargets ) );

	HandlerFunction getMpuSettings = std::bind( &Server::handleGetMpuSettings, this );
	m_impl.on( "/mpu/settings", HTTPMethod::HTTP_GET, std::bind( &handleRequest, m_display, getMpuSettings ) );

	HandlerFunction putMpuSettings = std::bind( &Server::handlePutMpuSettings, this );
	m_impl.on( "/mpu/settings", HTTPMethod::HTTP_PUT, std::bind( &handleRequest, m_display, putMpuSettings ) );

	HandlerFunction getMpuCalibration = std::bind( &Server::handleGetMpuCalibration, this );
	m_impl.on( "/mpu/calibration", HTTPMethod::HTTP_GET, std::bind( &handleRequest, m_display, getMpuCalibration ) );

	HandlerFunction doMpuCalibration = std::bind( &Server::handlePutMpuCalibration, this );
	m_impl.on( "/mpu/calibration", HTTPMethod::HTTP_PUT, std::bind( &handleRequest, m_display, doMpuCalibration ) );

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


void Server::handleGetMpuSettings()
{
	int32_t pitchOffset;
	if ( !m_motors->getMpuOffset(&pitchOffset) ) {
		TRACE_ERROR( "Error reading MPU settings" );
		sendError( m_impl );
		return;
	}

	jsonBuffer.clear();
	JsonObject& ret = jsonBuffer.createObject();
	ret["pitchOffset"] = ((float)pitchOffset) / 0x10000;		//Convert Q16 to float

	sendJson( m_impl, ret );
}


void Server::handlePutMpuSettings()
{
	jsonBuffer.clear();
	String body = m_impl.arg("plain");
	const JsonObject& root = jsonBuffer.parseObject( body );

	float pitchOffset;
	pitchOffset = root["pitchOffset"];
	if ( !m_motors->setMpuOffset( pitchOffset*0x10000 ) ) {		//Convert float to Q16
		TRACE_ERROR( "Error setting MPU settings" );
		sendError( m_impl );
		return;
	}

	sendNoContent( m_impl );
}



void Server::handleGetMpuCalibration()
{
	jsonBuffer.clear();
	JsonObject& ret = jsonBuffer.createObject();

	JsonObject& accel = ret.createNestedObject("accel");
	accel["x"] = m_mpu->getAccelCalibration()[0];
	accel["y"] = m_mpu->getAccelCalibration()[1];
	accel["z"] = m_mpu->getAccelCalibration()[2];

	JsonObject& gyro = ret.createNestedObject("gyro");
	gyro["x"] = m_mpu->getGyroCalibration()[0];
	gyro["y"] = m_mpu->getGyroCalibration()[1];
	gyro["z"] = m_mpu->getGyroCalibration()[2];

	sendJson( m_impl, ret );
}


void Server::handlePutMpuCalibration()
{
	m_motors->pause();

	i2c::init( i2c::MPU_SDA, i2c::MPU_SCL );		//Change I2C channel
	m_mpu->calibrate();

	i2c::init( i2c::MOTORS_SDA, i2c::MOTORS_SCL );	//Recover I2C channel
	m_motors->resume();

	handleGetMpuCalibration();						//Send calibration data
}


void Server::handlePutTargets()
{
	jsonBuffer.clear();
	String body = m_impl.arg("plain");
	const JsonObject& root = jsonBuffer.parseObject( body );

	int16_t speed = root["speed"];
	if ( !m_motors->speed().setTarget( speed ) ) {
		TRACE_ERROR( "Error setting speed target" );
		sendError( m_impl );
		return;
	}

	int16_t heading = root["heading"];
	if ( !m_motors->heading().setTarget( heading ) ) {
		TRACE_ERROR( "Error setting heading target" );
		sendError( m_impl );
		return;
	}

	sendNoContent( m_impl );
}


void Server::handleRoot()
{
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


void Server::handleNotFound()
{
	m_impl.send( 404, "text/html", "<h1>Resource not found</h1>" );
}


void Server::handleOptionsRequest()
{
	m_impl.sendHeader( ACCESS_CONTROL_ALLOW_ORIGIN, "*" );

	String requestHeader = m_impl.header(ACCESS_CONTROL_REQUEST_HEADERS);
	if ( requestHeader.length() > 0 ) {
		m_impl.sendHeader( ACCESS_CONTROL_ALLOW_HEADERS, requestHeader );
	}

	requestHeader = m_impl.header(ACCESS_CONTROL_REQUEST_METHOD);
	m_impl.sendHeader( ACCESS_CONTROL_ALLOW_METHODS, (requestHeader.length()>0) ? requestHeader : "GET, POST, PUT, OPTIONS" );

	m_impl.send( 200, "text/plain", "" );
}


}



