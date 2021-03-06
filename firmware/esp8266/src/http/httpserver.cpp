#include "http/httpserver.h"
#include "http/webhandlers.h"
#include "http/streamresponse.h"
#include "http/pidstatesstream.h"
#include "http/basicresponses.h"
#include "util/trace.h"
#include "i2c/i2c.h"
#include "util/arduino_stl_support.h"
#include "motors_i2c_model.h"
#include <ArduinoJson.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdio.h>



extern "C" {

//Declared in espconn.h but we can't include here due to type conflicts.
sint8 espconn_tcp_set_max_con(uint8 num);

}


namespace http
{

#define ACCESS_CONTROL_ALLOW_ORIGIN "Access-Control-Allow-Origin"
#define ACCESS_CONTROL_REQUEST_HEADERS "Access-Control-Request-Headers"
#define ACCESS_CONTROL_REQUEST_METHOD "Access-Control-Request-Method"
#define ACCESS_CONTROL_ALLOW_HEADERS "Access-Control-Allow-Headers"
#define ACCESS_CONTROL_ALLOW_METHODS "Access-Control-Allow-Methods"



/************************************************************************/
// PidService
/************************************************************************/
PidService::PidService( AsyncWebServer* server, const std::string& path,
						motion::PidEngine* pidEngine, io::Display* display ):
						m_pidEngine(pidEngine) {

	AsyncMethodWebHandler<PidService>::Factory handlers(this, display);

	std::string statePath = path + "/state";
	server->addHandler( handlers.create(statePath.c_str(), HTTP_GET, &PidService::handleState) );

	std::string settingsPath = path + "/settings";
	server->addHandler( handlers.create(settingsPath.c_str(), HTTP_GET, &PidService::handleSettings) );

	server->addHandler( handlers.create(settingsPath.c_str(), HTTP_PUT, &PidService::handleSetSettings) );
}


void PidService::handleState( AsyncWebServerRequest *request ) {
	static const uint8_t MAX_STATES_SIZE = 150;		//Limit size to prevent RAM from running out

	std::unique_ptr<std::vector<::PIDStateEntry>> states( new std::vector<::PIDStateEntry>() );
	states->reserve(MAX_STATES_SIZE);
	if ( !m_pidEngine->state( *states, MAX_STATES_SIZE ) ) {
		TRACE_ERROR( F("Error reading PID states") );
		sendError(request);
		return;
	}

	PidStatesStream* statesStream = new PidStatesStream( std::move(states) );
	request->onDisconnect( [statesStream]() {
		delete statesStream;
	});

	request->send( new AsyncNoTemplateStreamResponse( statesStream, CONTENT_TYPE_JSON, statesStream->available() ) );
}


void PidService::handleSettings( AsyncWebServerRequest *request ) {
	::PIDSettings settings;
	if ( !m_pidEngine->settins(settings) ) {
		TRACE_ERROR( F("Error reading PID settings") );
		sendError(request);
		return;
	}

	const int BUFFER_SIZE = JSON_OBJECT_SIZE(3) ;
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
	JsonObject &jsonSetings = jsonBuffer.createObject();
	jsonSetings["integral"] = settings.k_i;
	jsonSetings["proportional"] = settings.k_p;
	jsonSetings["derivative"] = settings.k_d;

	sendBasicJson( request, jsonSetings );
}


void PidService::handleSetSettings( AsyncWebServerRequest *request, char* data, size_t len ) {
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 1 );

	motion::Motors::PIDSettings settings;
	settings.k_i = root["integral"];
	settings.k_p = root["proportional"];
	settings.k_d = root["derivative"];

	if ( !m_pidEngine->setSettins(settings) ) {
		TRACE_ERROR( F("Error settings pitch PID settings") );
		sendError( request );
		return;
	}

	sendNoContent( request );
}



/************************************************************************/
// Server
/************************************************************************/
Server::Server( motion::Motors* motors, mpu::Mpu9250* mpu, io::Display* display ):
				m_motors(motors), m_display(display), m_mpu(mpu), m_impl(80)
{
	//limits the number of connections to prevent RAM from running out
	//This is no longer necessary with AsyncNoTemplateStreamResponse and limiting
	//the states vector size (see PidService::handleState)
//	espconn_tcp_set_max_con(2);

	m_wifiService.reset( new WifiService(&m_impl, "/rest/wifi", m_display) );

	m_pitchService.reset( new PidService( &m_impl, "/rest/pitch", &m_motors->pitch(), m_display ) );
	m_speedService.reset( new PidService( &m_impl, "/rest/speed", &m_motors->speed(), m_display ) );
	m_headingService.reset( new PidService( &m_impl, "/rest/heading", &m_motors->heading(), m_display ) );

	AsyncMethodWebHandler<Server>::Factory handlers(this, display);
	m_impl.addHandler( handlers.create("/rest/targets", HTTP_PUT, &Server::handlePutTargets ) );
	m_impl.addHandler( handlers.create("/rest/mpu/settings", HTTP_GET, &Server::handleGetMpuSettings ) );
	m_impl.addHandler( handlers.create("/rest/mpu/settings", HTTP_PUT, &Server::handlePutMpuSettings ) );
	m_impl.addHandler( handlers.create("/rest/mpu/calibration", HTTP_GET, &Server::handleGetMpuCalibration ) );
	m_impl.addHandler( handlers.create("/rest/mpu/calibration", HTTP_PUT, &Server::handlePutMpuCalibration ) );

	m_impl.onNotFound([this](AsyncWebServerRequest* request){
			if ( request->method() == HTTP_OPTIONS ) {
				handleOptionsRequest(request);
			}
			else {
				handleDefault( request );
			}
		});

	SPIFFS.begin();
	m_impl.serveStatic("/", SPIFFS, "/app").setDefaultFile("index.html");

	DefaultHeaders::Instance().addHeader(ACCESS_CONTROL_ALLOW_ORIGIN, "*");

	m_impl.begin();
}


void Server::handleDefault( AsyncWebServerRequest* request )
{
	if ( request->url().startsWith("/rest") || (request->method()!=HTTP_GET) ) {
		request->send( 404, "text/html", "<h1>Resource not found</h1>" );
		return;
	}
	AsyncWebServerResponse* response = new AsyncFileResponse(SPIFFS, "/app/index.html");
	request->send(response);
}


void Server::handleGetMpuSettings( AsyncWebServerRequest* request )
{
	int32_t pitchOffset;
	if ( !m_motors->getMpuOffset(&pitchOffset) ) {
		TRACE_ERROR( F("Error reading MPU settings") );
		sendError( request );
		return;
	}

	StaticJsonBuffer<JSON_OBJECT_SIZE(1)> jsonBuffer;
	JsonObject& ret = jsonBuffer.createObject();
	ret["pitchOffset"] = ((float)pitchOffset) / 0x10000;		//Convert Q16 to float

	sendBasicJson( request, ret );
}


void Server::handlePutMpuSettings( AsyncWebServerRequest* request, char* data, size_t len )
{
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 1 );

	float pitchOffset = root["pitchOffset"];
	if ( !m_motors->setMpuOffset( pitchOffset*0x10000 ) ) {		//Convert float to Q16
		TRACE_ERROR( F("Error setting MPU settings") );
		sendError( request );
		return;
	}

	sendNoContent( request );
}



void Server::handleGetMpuCalibration( AsyncWebServerRequest* request )
{
	StaticJsonBuffer<JSON_OBJECT_SIZE(2) * JSON_OBJECT_SIZE(3)> jsonBuffer;
	JsonObject& ret = jsonBuffer.createObject();

	JsonObject& accel = ret.createNestedObject("accel");
	accel["x"] = m_mpu->getAccelCalibration()[0];
	accel["y"] = m_mpu->getAccelCalibration()[1];
	accel["z"] = m_mpu->getAccelCalibration()[2];

	JsonObject& gyro = ret.createNestedObject("gyro");
	gyro["x"] = m_mpu->getGyroCalibration()[0];
	gyro["y"] = m_mpu->getGyroCalibration()[1];
	gyro["z"] = m_mpu->getGyroCalibration()[2];

	sendBasicJson( request, ret );
}


void Server::handlePutMpuCalibration( AsyncWebServerRequest* request )
{
	m_motors->pause();

	i2c::init( i2c::MPU_SDA, i2c::MPU_SCL );		//Change I2C channel
	m_mpu->calibrate();

	i2c::init( i2c::MOTORS_SDA, i2c::MOTORS_SCL );	//Recover I2C channel
	m_motors->resume();

	handleGetMpuCalibration( request );				//Send calibration data
}


void Server::handlePutTargets( AsyncWebServerRequest* request, char* data, size_t len )
{
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 1 );

	int16_t speed = root["speed"];
	if ( !m_motors->speed().setTarget( speed ) ) {
		TRACE_ERROR( F("Error setting speed target") );
		sendError( request );
		return;
	}

	int16_t heading = root["heading"];
	if ( !m_motors->heading().setTarget( heading ) ) {
		TRACE_ERROR( F("Error setting heading target") );
		sendError( request );
		return;
	}

	sendNoContent( request );
}


void Server::handleOptionsRequest( AsyncWebServerRequest *request )
{
	AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", "");

	String requestHeader = request->header(ACCESS_CONTROL_REQUEST_HEADERS);
	if ( requestHeader.length() > 0 ) {
		response->addHeader( ACCESS_CONTROL_ALLOW_HEADERS, requestHeader );
	}

	requestHeader = request->header(ACCESS_CONTROL_REQUEST_METHOD);
	response->addHeader( ACCESS_CONTROL_ALLOW_METHODS, (requestHeader.length()>0) ? requestHeader : "GET, POST, PUT, OPTIONS" );

	request->send( response );
}


}



